#include "xcall_jit.h"
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/TargetSelect.h>
#include <vector>

using namespace llvm;
using namespace llvm::orc;

std::unique_ptr<LLJIT> llbuilder;

ExitOnError exitOnError;
InitLLVM* initllvm;

//--------------------------------------------------------------------
// returns error code on failure, otherwise returns 0 for success.
int init_jit(int argc, char* argv[])
{
	initllvm = new InitLLVM(argc, (char**&)argv, false);
	InitializeNativeTarget();
	InitializeNativeTargetAsmPrinter();
	llbuilder = exitOnError(LLJITBuilder().create());
	
	return 0;
}
//--------------------------------------------------------------------
// the first THREE parameters MUST be (cdts[2], char**, uint64_t*), only after that it can be constant
pforeign_function_entrypoint_signature_params_ret_t create_xcall_params_ret(const std::vector<uint32_t>& constant_sizes, const std::vector<int64_t>& constant_values, const char* function_path, void* func)
{
	auto create = [&]()->ThreadSafeModule
	{
		auto context = std::make_unique<LLVMContext>();
		auto module = std::make_unique<Module>("module", *context);
		
		// create the new function
		FunctionCallee new_func_callee = module->getOrInsertFunction(function_path,
								    FunctionType::get(Type::getVoidTy(*context),
                                            {Type::getInt64PtrTy(*context), Type::getInt8PtrTy(*context), Type::getInt64PtrTy(*context)},
                                            false));

		Function* new_func = dyn_cast<Function>(new_func_callee.getCallee());

		BasicBlock* new_func_block = BasicBlock::Create(*context, "EntryBlock", new_func);
		IRBuilder<> builder(new_func_block);

		// place constants
		size_t constants_len = constant_sizes.size();
		std::vector<Value*> constant_params;
		for (int i = 0; i < constants_len; i++)
		{
			constant_params.push_back(builder.getIntN(constant_sizes[i], (uint64_t)constant_values[i]));
		}
		
		auto arg_it = new_func->arg_begin();
		Value* pcdts = &(*arg_it);
		arg_it++;
		Value* err = &(*arg_it);
		arg_it++;
		Value* err_len = &(*arg_it);

		std::vector<Value*> args = {pcdts, err, err_len};
		args.insert(args.end(), constant_params.begin(), constant_params.end());
		
		std::vector<Type*> param_types = {Type::getInt64PtrTy(*context), Type::getInt8PtrTy(*context), Type::getInt64PtrTy(*context)};
		for (uint32_t size: constant_sizes)
		{
			param_types.push_back(Type::getIntNTy(*context, size));
		}
		
		ConstantInt* pfunc = builder.getInt64((uint64_t)func);
		
		// call original function
		builder.CreateCall(FunctionType::get(Type::getVoidTy(*context),
		                                     ArrayRef(param_types),
		                                     false),
									    pfunc,
                                        ArrayRef(args));

		builder.CreateRetVoid();

		return ThreadSafeModule(std::move(module), std::move(context));
	};
	
	exitOnError(llbuilder->addIRModule(std::move(create())));
	auto expect_symbol = exitOnError(llbuilder->lookup(function_path));
	
	pforeign_function_entrypoint_signature_params_ret_t pnewfunc = (pforeign_function_entrypoint_signature_params_ret_t)expect_symbol.getAddress();
	
	return pnewfunc;
}
//--------------------------------------------------------------------
