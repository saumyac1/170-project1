import heapq
import copy

class TreeNode:
    def __init__(self, parent, puzzle, cost, h_n):
        self.parent = parent # Referring to parent node
        self.puzzle = puzzle # Current puzzle
        self.cost = cost # G(n) - cost of path to reach current node
        self.h_n = h_n # H(n) - heuristic value

    # Compare cost of two nodes
    def __lt__(self, other):
        return self.cost < other.cost
    
    # Covert puzzle to tuple format
    def puzzle_to_tuple(self):
        return tuple(tuple(row) for row in self.puzzle)
    
    # Find blank space in puzzle
    def blank(self):
        for i in range(3):
            for j in range(3):
                if self.puzzle[i][j] == 0:
                    return i, j
        return
    
    # Generate neighbors of current node
    def get_neighbors(self, heuristic):
        neighbors = []
        r, c = self.blank() # Find row and column of blank space
        moves = [(0, -1), (0, 1), (-1, 0), (1, 0)] # left, right, up, down

        # Try all possible moves
        for dr, dc in moves:
            new_r, new_c = r + dr, c + dc
            if 0 <= new_r < 3 and 0 <= new_c < 3: # Making sure move is within bounds
                new_puzzle = copy.deepcopy(self.puzzle) # Create copy of puzzle
                new_puzzle[r][c], new_puzzle[new_r][new_c] = new_puzzle[new_r][new_c], new_puzzle[r][c] # Swap blank with tile
                new_cost = self.cost + 1 # Increment cost by 1

                # Calculate heuristic value for new puzzle, depending on which one
                new_h_n = 0
                if heuristic == "Misplaced Tile Heuristic":
                    new_h_n = misplaced_tile_heuristic(new_puzzle)
                elif heuristic == "Manhattan Distance Heuristic":
                    new_h_n = manhattan_distance_heuristic(new_puzzle)                

                # Create new node with new puzzle, cost, and heuristic value, and add to list
                neighbor_node = TreeNode(self, new_puzzle, new_cost, new_h_n)
                neighbors.append(neighbor_node)
        return neighbors

# Main function
def main():
    puzzle_type = input("Welcome to my 8-Puzzle Solver! \nType \'1\' to use a default puzzle.\nType \'2\' to create your own.\n")
    
    # Keeps asking for input until valid input is given
    while puzzle_type not in ('1', '2'):
        print("INVALID INPUT. Please enter '1' or '2':")
        puzzle_type = input("\nType \'1\' to use a default puzzle.\nType \'2\' to create your own.\n")
    
    if puzzle_type == '1':
        select_algorithm(puzzle_difficulty())
    else:
        select_algorithm(create_puzzle())

    return


# Function to select difficulty of puzzle
def puzzle_difficulty():
    selected_difficulty = input("You wish to use a default puzzle.\nPlease enter a desired difficulty on a scale from 1 to 5." + '\n')
    if selected_difficulty == "1":
        print("\nDifficulty of 'Trivial' selected.")
        return trivial
    if selected_difficulty == "2":
        print("\nDifficulty of 'Very Easy' selected.")
        return very_easy
    if selected_difficulty == "3":
        print("\nDifficulty of 'Easy' selected.")
        return easy
    if selected_difficulty == "4":
        print("\nDifficulty of 'Doable' selected.")
        return doable
    if selected_difficulty == "5":
        print("\nDifficulty of 'Oh Boy' selected.")
        return oh_boy

# Function to create custom puzzle
def create_puzzle():
    while True:
        print("\nEnter your puzzle, using a zero to represent the blank.\nPlease only enter valid 8-puzzles.\nEnter the puzzle separating the numbers with a space.\n")
        row1 = input("Enter the first row: ")
        row2 = input("Enter the second row: ")
        row3 = input("Enter the third row: ")

        row1 = row1.split()
        row2 = row2.split()
        row3 = row3.split()

        for i in range(0, 3):
            row1[i] = int(row1[i])
            row2[i] = int(row2[i])
            row3[i] = int(row3[i])
        
        user_puzzle = [row1, row2, row3]

        if is_valid_puzzle(user_puzzle):
            break
        else:
            print("INVALID PUZZLE. TRY AGAIN.")
    return user_puzzle

# Function to print puzzle
def print_puzzle(puzzle):
    for i in range(0, 3):
        print(puzzle[i])
    print('\n')

# Function to select algorithm
def select_algorithm(puzzle):
    algorithm = input("Select algorithm.\n(1) for Uniform Cost Search\n(2) for Misplaced Tile Heuristic\n(3) the Manhattan Distance Heuristic.\n")
    while algorithm not in ('1', '2', '3'):
        print("INVALID INPUT. Please enter '1', '2', or '3':\n")
        algorithm = input("Select algorithm. (1) for Uniform Cost Search, (2) for Misplaced Tile Heuristic, or (3) the Manhattan Distance Heuristic.\n")
    
    h_n = 0
    initial_node = TreeNode(None, puzzle, 0, h_n)

    # Call uniform cost search based on selected algorithm
    if algorithm == '1':
        uniform_cost_search(puzzle, 0, "Uniform Cost Search")
    elif algorithm == '2':
        uniform_cost_search(puzzle, misplaced_tile_heuristic(puzzle), "Misplaced Tile Heuristic")
    else:
        uniform_cost_search(puzzle, manhattan_distance_heuristic(puzzle), "Manhattan Distance Heuristic")

# Function to check if puzzle is valid
def is_valid_puzzle(puzzle):
    # Check for duplicates
    flattened = [num for row in puzzle for num in row]
    if sorted(flattened) != list(range(9)):
        return False
    
    # Check for solvability
    inversions = 0
    for i in range(9):
        for j in range(i + 1, 9):
            if (flattened[j] != 0 and flattened[i] != 0 and flattened[i] > flattened[j]):
                inversions += 1
    
    return inversions % 2 == 0

# Check if puzzle is goal state
def is_goal_state(puzzle):
    return puzzle == eight_goal_state

# Uniform cost search algorithm
def uniform_cost_search(initial, h_n, h_n_name):
    initial_node = TreeNode(None, initial, 0, h_n) # Create initial node w/ cost 0 and initial heuristic value
    current_queue = [] # Priority queue to hold nodes to be explored
    repeated_states = {} # Dictionary to keep track of repeated states
    heapq.heappush(current_queue, initial_node) # Push initial node to queue
    num_nodes_expanded = 0  # Number of nodes expanded
    max_queue_size = 0 # To Track maximum queue size

    repeated_states[initial_node.puzzle_to_tuple()] = initial_node.cost # Mark initial puzzle as visited

    while current_queue:
        max_queue_size = max(max_queue_size, len(current_queue)) # Track maximum size of queue
        current_node = heapq.heappop(current_queue) # Pop node with lowest cost

        if is_goal_state(current_node.puzzle): # Check if goal state is reached
            solution_path = [] # Reconstruct solution path
            while current_node:
                solution_path.append(current_node) # Add node to solution path
                current_node = current_node.parent  # Move to parent node
            solution_path.reverse()     # Reverse solution path to get correct order
            for node in solution_path:
                print("The best state to expand with a g(n) = 1 and h(n) = ", node.h_n, " is...")
                print_puzzle(node.puzzle)
            print("Solution depth: ", solution_path[-1].cost)
            print("Number of nodes expanded: ", num_nodes_expanded)
            print("Max queue size: ", max_queue_size)
            return solution_path
        num_nodes_expanded += 1

        # Generate neighbors of current node
        for neighbor in current_node.get_neighbors(h_n_name):
            neighbor_tuple = neighbor.puzzle_to_tuple()
            if neighbor_tuple not in repeated_states or neighbor.cost < repeated_states[neighbor_tuple]:
                repeated_states[neighbor_tuple] = neighbor.cost
                heapq.heappush(current_queue, neighbor)

# Heuristic for counting misplaced tiles
def misplaced_tile_heuristic(initial):
    sum = 0
    for i in range(3):
        for j in range(3):
            if initial[i][j] != 0 and initial[i][j] != eight_goal_state[i][j]:
                sum += 1
    return sum

# Heuristic for calculating Manhattan distance
def manhattan_distance_heuristic(initial):
    distance = 0
    for i in range(3):
        for j in range(3):
            tile = initial[i][j]
            if tile != 0:
                r_goal, c_goal = divmod(tile - 1, 3) # Find goal position for tile
                distance += abs(i - r_goal) + abs(j - c_goal)
    return distance

###

# Default puzzles
trivial = [[1, 2, 3],
           [4, 5, 6],
           [7, 8, 0]]
very_easy = [[1, 2, 3],
             [4, 5, 6],
             [7, 0, 8]]
easy = [[1, 2, 0],
        [4, 5, 3],
        [7, 8, 6]]
doable = [[0, 1, 2],
          [4, 5, 3],
          [7, 8, 6]]
oh_boy = [[8, 7, 1],
          [6, 0, 2],
          [5, 4, 3]]
eight_goal_state = [[1, 2, 3],
                    [4, 5, 6],
                    [7, 8, 0]]

###

# Main function call
main()
