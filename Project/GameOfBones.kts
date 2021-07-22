#! kotlinc -script
// https://kotlinlang.org/docs/command-line.html#run-scripts
// https://www.baeldung.com/java-minimax-algorithm

object GameOfBones {
	fun getPossibleStates(noOfBonesInHeap: Int): List<Integer> {
		return (1..3)
			.map { noOfBonesInHeap - it }
			.filter { it >= 0 }.toList()
	}


	fun constructTree(noOfBones: Int): Tree {
		val root = Node(noOfBones, true)
		val tree = Tree(root)
		constructTree(root)
		return tree
	}

	private fun constructTree(parentNode: Node) {
		val listofPossibleHeaps = GameOfBones.getPossibleStates(parentNode.noOfBones)
		val isChildMaxPlayer: Boolean = !parentNode.isMaxPlayer()
		listofPossibleHeaps.forEach { n ->
			val newNode = Node(n, isChildMaxPlayer)
			parentNode.addChild(newNode)
			if (newNode.noOfBones > 0) {
				constructTree(newNode)
			}
		}
	}

}

class Node(
	var noOfBones: Int = 0,
	var isMaxPlayer: Boolean = false,
	var score: Int = 0,
	var children: List<Node> = emptyList(),
)

class Tree(
	var root: Node,
)

class MiniMax(
	var tree: Tree
) {
	fun checkWin(): Boolean {
		val root: Node = tree.root
		checkWin(root)
		return root.score == 1
	}

	private fun checkWin(node: Node) {
		val children: List<Node> = node.children
		val isMaxPlayer: Boolean = node.isMaxPlayer
		children.forEach { child ->
			if (child.noOfBones === 0) {
				child.score = if (isMaxPlayer) 1 else -1
			} else {
				checkWin(child)
			}
		}
		val bestChild: Node = findBestChild(isMaxPlayer, children)
		node.score = bestChild.score
	}

	private fun findBestChild(isMaxPlayer: Boolean, children: List<Node>): Node {
		return children.maxByOrNull { it.score } ?: throw Exception()
	}
}


val miniMax6 = MiniMax(GameOfBones.constructTree(6))
val result6 = miniMax.checkWin()

val miniMax8 = MiniMax(GameOfBones.constructTree(8))
val result8 = miniMax.checkWin()
