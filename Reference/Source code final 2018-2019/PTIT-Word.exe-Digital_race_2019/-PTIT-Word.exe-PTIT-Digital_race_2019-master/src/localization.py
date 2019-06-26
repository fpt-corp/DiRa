import math

class Position:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def __str__(self):
        return "x: {} y: {} z: {}"\
            .format(self.x, self.y, self.z)


class CheckPoint:
    def __init__(self, name, position):
        self.position = position
        self.name = name
        self.neighbors = []

    def add_neighbors(self, *args):
        for cp in args:
            self.neighbors.append(cp)

    def __str__(self):
        return self.name


class CarPose:
    def __init__(self, checkpoint, param):
        self.position = Position(0, 0, 0)
        self.last_checkpoint = checkpoint
        self.param = param
        # list_state got at the beginning
        self.list_state = {}
        self.current_state = {}     # last node must have movement/action ?
        self.list_state_bak = {}

        self.turn = 's'             # 'l', 'r', 's'

        self.min_distance_to_checkpoint = 300


    # call after dijkstra-path-planning
    def update_list_state(self, list_state):
        self.list_state = list_state
        self.list_state_bak = list(list_state)
        self.update_current_state()

    def update_position(self, position):
        self.position = position
        self.update()

    def distance(self, pos1, pos2):
        return math.sqrt((pos1.x - pos2.x)**2 + (pos1.y - pos2.y)**2 + (pos1.z - pos2.z)**2)

    def update_current_state(self):
        for i, state in enumerate(self.list_state):
            if state["start"] == self.last_checkpoint.name:
                self.current_state = state
                self.list_state.pop(i)
                break

    # call after having a new position
    def update(self):
        # update last checkpoint
        for cp in self.last_checkpoint.neighbors:
            d = self.distance(cp.position, self.position)
            print(d)

            # if car is near a new check-point
            if d <= self.min_distance_to_checkpoint:
                self.last_checkpoint = cp
                self.update_current_state()
                break

        # action
        d = self.distance(self.last_checkpoint.position, self.position)
        if d <= self.min_distance_to_checkpoint:
            # turn
            self.turn = self.current_state[str(self.last_checkpoint)]
        else:
            self.turn = 's'
            
    def reset(self):
        self.last_checkpoint = self.param.checkpoint1
        self.update_list_state(list(self.list_state_bak))

