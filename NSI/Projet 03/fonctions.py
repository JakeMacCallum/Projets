import random


def get_coords(table=None, orientation=None, name=None, bot=False):
    coords = (0, 0)
    size = get_size(name) - 1
    Checked = False
    while not Checked:
        if not bot:
            coords = input("Donner les coordonées : ")
            coords = (int(coords[1:]) - 1, ord(coords[0]) - 65)
        else:
            coords = (random.randint(0, 9), random.randint(0, 9))
        if 0 <= coords[0] + size * orientation[0] <= 9 and 0 <= coords[1] + size * orientation[1] <= 9:
            Checked = True
            for parts in range(get_size(name)):
                y = coords[0] + parts * orientation[0]
                x = coords[1] + parts * orientation[1]
                if table[y][x] != 0:
                    print("Les bateaux sont superposés !")
                    Checked = False
                    break
        elif not bot:
            print("Les coordonnées sont de format lettre colone - numero ligne\nExemple : A5\nLe bateau est de taille",
                  size)
    return coords


def get_orientation():
    Checked = False
    orientation = ()
    while not Checked:
        orientation = input("Donner l'orientation : ")
        Checked = True
        if orientation in ["v", "V", "vertical", "Vertical"]:
            orientation = (1, 0)
        elif orientation in ["h", "H", "horizontal", "Horizontal"]:
            orientation = (0, 1)
        else:
            print("Orientation de format V/vertical ou H/horizontal")
            Checked = False
    return orientation


def get_id(tile):
    if tile == 0:
        return "       "
    elif tile == 1:
        return "~~~~~~~"
    elif tile[1] == "Coulé" or tile == "Coulé":
        return " Coulé "
    elif tile[1] == True or tile == "Touché":
        return "XXXXXXX"
    elif tile[0] == "Contre-Torpilleurs-1":
        return "1111111"
    elif tile[0] == "Contre-Torpilleurs-2":
        return "2222222"
    else:
        return tile[0][0] * 7


def get_size(name):
    if name == "Porte-avions":
        return 5
    elif name == "Croiseur":
        return 4
    elif name[:-2] == "Contre-Torpilleurs":
        return 3
    else:
        return 2


def show_table(table):
    string = "\t\tA\t\tB\t\tC\t\tD\t\tE\t\tF\t\tG\t\tH\t\tI\t\tJ\n\t" + "_" * 81 + "\n"
    num_lines = 1
    for lines in table:
        sub_string = "\t" + "|       " * 10 + f"|\n{num_lines}\t"
        num_lines += 1
        for columns in lines:
            columns = get_id(columns)
            sub_string += f"|{columns}"
        string += sub_string + "|\n \t" + "|_______" * 10 + "|\n"
    return string


def create_boat(table, orientation, coords, name):
    boat = []
    for parts in range(get_size(name)):
        y = coords[0] + parts * orientation[0]
        x = coords[1] + parts * orientation[1]
        table[y][x] = [name, False]
        boat.append((y, x))
    return boat


def attack(attack_table, table, boats, ai_attack=False, coords=(0, 0)):
    if not ai_attack:
        Checked = False
        while not Checked:
            coords = input("Donner les coordonées : ")
            coords = (int(coords[1:]) - 1, ord(coords[0]) - 65)
            if 0 <= coords[0] <= 9 and 0 <= coords[1] <= 9:
                Checked = True
            else:
                print(
                    "Les coordonnées sont de format lettre colone - numero ligne\n"
                    "Exemple : A5\n"
                    "Le bateau est de taille"
                )

    boat = None
    for i in boats:
        for j in i:
            if j == coords:
                boat = i
    counter = 0
    if table[coords[0]][coords[1]] == 0:
        table[coords[0]][coords[1]] = 1
        attack_table[coords[0]][coords[1]] = 1
        return "Raté"

    if not table[coords[0]][coords[1]][1]:
        for c in boat:
            if not table[c[0]][c[1]][1]:
                counter += 1
            if counter >= 2:
                table[coords[0]][coords[1]][1] = True
                attack_table[coords[0]][coords[1]] = "Touché"
                return "Touché"
        for c in boat:
            table[c[0]][c[1]] = ["Coulé", True]
            attack_table[c[0]][c[1]] = "Coulé"

        return "Coulé"
    return "What"


def auto_place(table, boat_list):
    x, y, orientation = 0, 0, (0, 0)
    boats = []
    for boat_name in boat_list:
        size = get_size(boat_name)
        valid = False
        while not valid:
            x = random.randint(0, 9)
            y = random.randint(0, 9)
            orientation = random.choice([(0, 1), (1, 0)])
            valid = valid_boat(size, table, x, y, orientation)

        boats.append(create_boat(table, orientation, (y, x), boat_name))

    return table, boats


def valid_boat(size, table, x, y, orientation):
    for i in range(size):
        for lines in range(-1, 2):
            for columns in range(-1, 2):
                test1 = (0 <= y + (i * orientation[0]) + lines <= 9
                         and 0 <= x + (i * orientation[1]) + columns <= 9
                         )
                if test1:
                    test2 = table[y + (i * orientation[0]) + lines][x + (i * orientation[1]) + columns] != 0
                    if test2:
                        return False
                else:
                    return False
    return True


def ai(boat_list, attack_table):
    possible_locations = []
    for boat_name in boat_list:
        size = get_size(boat_name)
        for y in range(10):
            for x in range(10):
                for orientation in [(0, 1), (1, 0)]:
                    if (
                            (not valid_boat(size, attack_table, x, y, orientation))
                            or attack_table[y][x] in [1, "Touché", "Coulé"]
                    ):
                        pass
                    else:
                        possible_locations.append(
                            (boat_name, [(y + n * orientation[0], x + n * orientation[1]) for n in range(size)]))
    incompatible_locations = []
    for boat_name in boat_list:
        for locations in possible_locations:
            if possible_locations[0] == boat_name:
                for other_boat_name in boat_list:
                    if other_boat_name != boat_name:
                        for other_locations in possible_locations:
                            if possible_locations[0] == other_boat_name:
                                for coords in locations[1]:
                                    for other_coords in other_locations[1]:
                                        if coords == other_coords:
                                            incompatible_locations.append((locations[1], other_locations[1]))
    locationFrequencies = []
    locationFrequencies = {}
    validConfigurationsCounted = 0

    for _ in range(1000):
        selected_locations = []
        for boat_name in boat_list:
            valid_location_found = False
            while not valid_location_found:
                L = [boat[1] for boat in possible_locations if boat[0] == boat_name]
                location = random.choice([i for i in L])
                if all(location not in incompatible for incompatible in incompatible_locations):
                    valid_location_found = True
                    selected_locations.append(location)

        if conflicts_with_board_state(selected_locations, attack_table):
            continue

        for location in selected_locations:
            for coord in location:
                locationFrequencies[coord] = locationFrequencies.get(coord, 0) + 1
        validConfigurationsCounted += 1

    squareFrequencies = {}
    for data in possible_locations:
        for square in data[1]:
            if square in list(locationFrequencies.keys()):
                squareFrequencies[square] = squareFrequencies.get(square, 0) + locationFrequencies[square]

    for square, frequency in squareFrequencies.items():
        squareFrequencies[square] /= validConfigurationsCounted
    best_square = max(squareFrequencies, key=squareFrequencies.get)

    return best_square


def conflicts_with_board_state(selected_locations, attack_table):
    for location in selected_locations:
        for coord in location:
            y, x = coord
            if not (0 <= y <= 9 and 0 <= x <= 9) or attack_table[y][x] in [1, "Touché", "Coulé"]:
                return True
    return False


def ai_attack(attack_table):
    orientation = ()
    attack_coords = []
    hit_coords = []
    chosen_hit = None
    for i in range(10):
        for j in range(10):
            if attack_table[i][j] in [1, "Touché", "Coulé"]:
                attack_coords.append((i, j))
            if attack_table[i][j] == "Touché":
                hit_coords.append((i, j))
    if not hit_coords:
        Check = False
        while not Check:
            chosen_hit = random.randint(0, 9), random.randint(0, 9)
            if chosen_hit not in attack_coords:
                Check = True
        return chosen_hit
    else:
        adjacent_coords = []
        temp_coord = None
        for coord in hit_coords:
            for y in range(-1, 2):
                for x in range(-1, 2):
                    if abs(x) != abs(y):
                        temp_coord = coord[0] + y, coord[1] + x
                        if valid_coord(temp_coord) and temp_coord not in attack_coords:
                            adjacent_coords.append(temp_coord)
        if adjacent_coords:
            temp_coord = hit_coords[0]
            for coords in hit_coords[1:]:
                if abs(temp_coord[0] - coords[0]) <= 5 or abs(temp_coord[1] - coords[1]) <= 5:
                    if (abs(temp_coord[0] - coords[0]), abs(temp_coord[1] - coords[1])) == (
                            abs(temp_coord[0] - coords[0]), 0):
                        orientation = (1, 0)
                    elif (abs(temp_coord[0] - coords[0]), abs(temp_coord[1] - coords[1])) == (
                            0, abs(temp_coord[1] - coords[1])):
                        orientation = (0, 1)
                    else:
                        continue
                    Check = False
                    i = 0
                    j = 0
                    while not Check:
                        chosen_coord_1 = (coords[0] + i * orientation[0], coords[1] + i * orientation[1])
                        chosen_coord_2 = (coords[0] + j * orientation[0], coords[1] + j * orientation[1])
                        if valid_coord(chosen_coord_1) and chosen_coord_1 not in attack_coords:
                            return chosen_coord_1
                        elif valid_coord(chosen_coord_2) and chosen_coord_2 not in attack_coords:
                            return chosen_coord_2
                        i -= 1
                        if i == -10:
                            break

                temp_coord = coords
            chosen_hit = random.choice(adjacent_coords)
            return chosen_hit
        else:
            print("Error")
            Check = False
            while not Check:
                chosen_hit = random.randint(0, 9), random.randint(0, 9)
                if chosen_hit not in attack_coords:
                    Check = True
            return chosen_hit


def valid_coord(coord):
    return 0 <= coord[0] <= 9 and 0 <= coord[1] <= 9


"""
adjacent_coords = []
        temp_coord = None
        for coord in hit_coords:
            for y in range(-1, 2):
                for x in range(-1, 2):
                    if abs(x) != abs(y):
                        temp_coord = coord[0] + y, coord[1] + x
                        if valid_coord(temp_coord) and temp_coord not in attack_coords:
                            adjacent_coords.append(temp_coord)
        if adjacent_coords:
            chosen_hit = random.choice(adjacent_coords)
            return chosen_hit
        else:
            print("Error")
            Check = False
            while not Check:
                chosen_hit = random.randint(0, 9), random.randint(0, 9)
                if chosen_hit not in attack_coords:
                    Check = True
            return chosen_hit"""
