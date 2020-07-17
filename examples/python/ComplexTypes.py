
# Complex types that can be shared between languages in OpenFFI are
# the complex types defined in protobuf.
# The following example uses 2 complex types from PythonFuncs_pb2.py generated by protoc
# game_data{ player, pawn_wins, boss_wins }
# game_score{ player, score }
def calculate_score(gdata):
	score = gdata.pawn_wins
	score = score + (gdata.boss_wins * 5)
	print('{} score is {}'.format(gdata.player, score))

	gscore = PythonFuncs_pb2.game_score()
	gscore.player = gdata.player
	gscore.score = score

	return gscore