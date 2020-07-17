import PythonFuncs_pb2

# Complex types that can be shared between languages in OpenFFI are
# the complex types defined in protobuf.
# The following example uses 2 complex types from PythonFuncs_pb2.py generated by protoc
# game_data{ player, pawn_wins, boss_wins }
# game_score{ player, score }
def calculate_score(gdata : PythonFuncs_pb2.game_data):
	score = gdata.pawn_wins
	score = score + (gdata.boss_wins * 5)
	print('{} score is {}'.format(gdata.player, score))

	gscore = PythonFuncs_pb2.game_score()
	gscore.player = gdata.player
	gscore.score = score

	return gscore

def gen_scores():
	gscore1 = PythonFuncs_pb2.game_score()
	gscore1.player = 'player1'
	gscore1.score = 1

	gscore2 = PythonFuncs_pb2.game_score()
	gscore2.player = 'player2'
	gscore2.score = 5

	return [gscore1, gscore2], (gscore1.score + gscore2.score)
