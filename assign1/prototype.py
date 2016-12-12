class Model(object):
    def __init__(self, x, y):
        self.height = y
        self.width = x
        self.board = [ ["·" for i in range(y)] for i in range(x) ]

    def print_board(self):
        for row in self.board:
            for char in row:
                if char is None:
                    print(" " + "·" + " ", end="")
                else:
                    print(" " + str(char) + " ", end="")
            print("")

    def get_node(self, x, y):
        if not 0 <= x < self.width:
            raise Exception("OOB")

        if not 0 <= y < self.height:
            raise Exception("OOB")

        return self.board[y][x]

    def is_OOB(self, x, y):
        return not (0 <= x < self.width and 0 <= y < self.height)

    def set_node(self, x, y, v):
        if self.get_node(x, y) is not "·":
            raise Exception("Already occupied")

        if not 0 <= x < self.width:
            raise Exception("OOB")

        if not 0 <= y < self.height:
            raise Exception("OOB")

        self.board[y][x] = v

        if self.loss_condition_around(x, y):
            print("VICTORY")

        if self.loss_condition_at(x, y):
            print("SUICIDE")

    # Did the most recently placed pebble cause the game to end?
    def loss_condition_around(self, x, y):
        up = self.loss_condition_at(x, y - 1)
        down = self.loss_condition_at(x, y + 1)
        left = self.loss_condition_at(x - 1, y)
        right = self.loss_condition_at(x + 1, y)
        
        return any([up, down, left, right])

    # Is the coordinate part of a string that has no liberties
    def loss_condition_at(self, x, y):
        malloc = []

        # Walls aren't strings
        if self.is_OOB(x, y):
            return False

        # Neither are free spaces
        current_pos = self.get_node(x, y)
        if current_pos == "·":
            return False

        # But pebbles COULD be part of strings, so let's check
        # if the string has liberties
        # We'll be using the color of pebble at the current position
        # to make sure we keep exploring the same string
        result = not self.has_liberties(x, y, current_pos, malloc)

        # free(malloc)
        return result 

    def has_liberties(self, x, y, string_color, seen):
        # Base case 0
        # If a position has been seen before, it means the function didn't
        # terminate on that position
        # Thus, that position yielded no liberties, and thus has no liberties
        if (x, y) in seen:
            return False
        else:
            seen.append((x, y))

        # Base case 1
        # A wall is not a liberty
        if self.is_OOB(x, y):
            return False

        pos_to_check = self.get_node(x, y)

        # Base case 2
        # If a spot is free, it's a liberty
        # We're defining being a liberty, as having liberties
        # We'll hand that back up to the caller
        if pos_to_check == "·":
            return True
        # The recursion case
        # Here the position contains a pebble of the color the of string we're
        # checkig has liberties
        # Thus we just check if any neighbours (of the same colour)
        # have liberties, or are liberties
        elif pos_to_check == string_color:
            up = self.has_liberties(x, y - 1, pos_to_check, seen)
            down = self.has_liberties(x, y + 1, pos_to_check, seen)
            left = self.has_liberties(x - 1, y, pos_to_check, seen)
            right = self.has_liberties(x + 1, y, pos_to_check, seen)
            return any([up, down, left, right])
        # Base case 3
        # This happens when:
        # - The position is OOB (a wall)
        # - The positon is a pebble of a different colour of the one that we're
        #   currently checking
        # Both aren't liberties
        else:
            return False

# Main should not depend on Node
def main():
    model = Model(5, 5)
    model.print_board()

    model.set_node(1, 1, "X")
    model.set_node(0, 0, "O")
    model.set_node(1, 0, "X")
    model.set_node(0, 1, "O")

    # This should trigger a victory condition
    # model.set_node(0, 2, "X")

    print("")
    print("")
    model.print_board()

if __name__ == '__main__':
    main()