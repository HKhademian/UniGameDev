import math
import random

_INF = 999
_SIZE = 3
_PLAYER = [' X ', ' @ ']

cost = 0


def showBoard(node):
	size = math.isqrt(len(node))
	for i in range(len(node)):
		name = _PLAYER[node[i]] if node[i] is not None else f'({i})'
		print(name, end='\t')
		if (i + 1) % size == 0:
			print()


def getWinSets(node):
	size = int(math.isqrt(len(node)))

	# Check Main Axe
	diag = True
	for i in range(size):
		if node[0 * size + 0] != node[i * size + i]:
			diag = False
			break
	if diag and node[0 * size + 0] is not None:
		yield node[0 * size + 0] , [(i * size + i) for i in range(size)]

	# Check Sec Axe
	diag = True
	for i in range(size):
		if node[0 * size + (size - 1)] != node[i * size + (size - 1 - i)]:
			diag = False
			break
	if diag and node[0 * size + (size - 1)] is not None:
		yield node[0 * size + (size - 1)] , [(i * size + (size - 1 - i)) for i in range(size)]


	def checkRow(r):
		for c in range(size):
			if node[r * size + 0] != node[r * size + c]:
				return None
		return node[r * size + 0]

	def checkCol(c):
		for r in range(size):
			if node[0 * size + c] != node[r * size + c]:
				return None
		return node[0 * size + c]

	for j in range(size):
		res = checkRow(j)
		if res is not None:
			yield res, [(j * size + i) for i in range(size)]
		res = checkCol(j)
		if res is not None:
			yield res, [(i * size + j) for i in range(size)]

def checkWinner(node):
	for (winner,_) in getWinSets(node):
		return winner
	return None


def childNodes(node, player):
	moves = list(range(len(node)))
	# may add sort
	random.shuffle(moves)
	for i in moves:
		if node[i] is not None:
			continue  # skip illegal moves
		child = node.copy()
		child[i] = player  # player pick i
		yield i, child


def evalBoard(node, player):
	# a good guess is sum of all win moves
	# or neg sum of all loss moves
	res = 0
	for child in childNodes(node, player):
		winner = checkWinner(node)
		if winner == player:
			res += 2 # not evenly graded
		elif winner is not None: 
			res -= 1
	return res


def alphabet(node: list, player: int, depth: int, minimize: bool = False, limit: int = +_INF, puring=True):
	global cost
	cost += 1
	winner = checkWinner(node)
	if winner == player:
		return None, (+depth + 1)*3, depth
	if winner is not None:
		return None, (-depth - 1)*3, depth
	if isFull(node):
		return None, 0, depth
	if depth <= 0:
		return None, evalBoard(node, player), depth

	best = None
	childPlayer = player if not minimize else (1 - player)

	for (i, childNode) in childNodes(node, childPlayer):
		childLimit = best[1] if best is not None else -_INF if not minimize else +_INF
		res = alphabet(childNode, player=player, depth=(depth - 1), minimize=(not minimize), limit=childLimit)
		if best is None or (not minimize and res[1] > best[1]) or (minimize and res[1] < best[1]):
			best = (i, res[1], res[2])
		if puring and ((minimize and best[1] <= limit) or (not minimize and best[1] >= limit)):  # alphabet puring
			break

	return best


def playerMove(node, depth=+10, auto=False, puring=True):
	prevCost = cost
	choice, choice_val, choice_depth = alphabet(node, player=0, depth=depth, puring=puring)
	myCost = cost - prevCost
	print(f"Player calculated best move (cost={myCost}) is:")
	print(f"to pick {choice} with value={choice_val} in depth={depth-choice_depth}")
	if not auto:
		choice = None
		while choice is None:
			choice = int(input('Enter cell number to fill:'))
			if choice not in range(len(node)):
				print('Wrong input')
				choice = None
				continue
			if node[choice] is not None:
				print('Already picked, choose another')
				choice = None
				continue
	node[choice] = 0  # player picks choice cell
	print(f"Player pick {choice}")


def aiMove(node, depth=+10, puring=True):
	prevCost = cost
	choice, choice_val, choice_depth = alphabet(node, player=1, depth=depth, puring=puring)
	myCost = cost - prevCost
	print(f"AI calculated best move (cost={myCost}) is:")
	print(f"to pick {choice} with value={choice_val} in depth={depth-choice_depth}")
	node[choice] = 1  # ai picks choice cell
	print(f"AI pick {choice}")


def isFull(node):
	for cell in node:
		if cell is None:
			return False
	return True


def Game(puring = False, turn = 0, player_auto = True, ai_level = +_INF, player_level = +_INF):
	board: list = [None for _ in range(_SIZE * _SIZE)]
	
	while True:
		showBoard(board)
		print()

		if isFull(board):
			print("Withdraw")
			break

		winner = checkWinner(board)
		if winner == 0:
			print("Players Win")
			break
		elif winner == 1:
			print("AI wins")
			break

		if turn == 0:
			playerMove(board, depth=player_level, auto=player_auto, puring=puring)
		else:
			aiMove(board, depth=ai_level, puring=puring)
		turn = 1 - turn

	print(f"total cost: {cost}")


if __name__ == '__main__':
	print("TicTacToe")
	print("@=AI    X=USER")
	print("")
	
	puring = True
	turn = 0  # int(input('choose who picks first [0:player, 1:ai]: '))
	player_auto = False  # int(input('player mode [0:input , 1:auto]: '))
	
	# all possible serial moves is 9 so depth=10 is like +INF
	ai_level = +10  # int(input('enter ai level [1-10]: '))
	player_level = +10  # int(input('enter guid level [1-10]: '))

	# while True:
	Game(puring, turn, player_auto, ai_level, player_level)
