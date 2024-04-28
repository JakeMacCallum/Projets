import fonctions as f

global width, height, player, opponent, player_attack_table, opponent_attack_table, \
    boat_list, player_boats, opponent_boats, opponent_win, playing, win, running


def start():
    global width, height, player, opponent, player_attack_table, opponent_attack_table, \
        boat_list, player_boats, opponent_boats, opponent_win, playing, win, running
    width, height = 10, 10
    player = [[0 for _ in range(width)] for _ in range(height)]
    opponent = [[0 for _ in range(width)] for _ in range(height)]
    player_attack_table = [[0 for _ in range(width)] for _ in range(height)]
    opponent_attack_table = [[0 for _ in range(width)] for _ in range(height)]
    boat_list = [
        "Porte-avions",
        "Croiseur",
        "Contre-Torpilleurs-1",
        "Contre-Torpilleurs-2",
        "Torpilleur"
    ]
    player_boats = []
    opponent_boats = []

    opponent_win = 0

    playing = False
    win = 0

    running = True


start()
while running:
    if win == 0:
        if playing:
            print("Votre table")
            print(player)
            coords = f.ai_attack(opponent_attack_table)
            resultat = f.attack(opponent_attack_table, player, player_boats, ai_attack=True, coords=coords)
            print(f.show_table(player))
            print(resultat)
            if resultat == "Coulé":
                test = True
                for boat in opponent_boats:
                    for coords in boat:
                        if opponent[coords[0]][coords[1]] != ["Coulé", True]:
                            test = False
                            print(resultat)
                            break
                if test:
                    win = -1

            print("Table de l'adversaire")
            print(f.show_table(player_attack_table))
            resultat = f.attack(player_attack_table, opponent, opponent_boats)
            print(resultat)
            if resultat == "Coulé":
                test = True
                for boat in opponent_boats:
                    for coords in boat:
                        if opponent[coords[0]][coords[1]] != ["Coulé", True]:
                            test = False
                            break
                if test:
                    win = 1
        else:
            placement_mode = input("Voulez-vous placer vos bateaux automatiquement ? OUI/oui/autre : ")
            if placement_mode in ["OUI", "oui"]:
                Happy = False
                while not Happy:
                    result = f.auto_place(player, boat_list)
                    player = result[0]
                    player_boats = result[1]
                    print(f.show_table(player))
                    if input("Ce placement convient ? (OUI/NON ou oui/non) : ") in ["OUI", "oui"]:
                        Happy = True
            else:
                for boats in boat_list:
                    print(f.show_table(player) + "\n\nPlacement d'un " + boats)
                    orientation = f.get_orientation()
                    player_boats.append(
                        f.create_boat(player, orientation, f.get_coords(player, orientation, boats), boats))
            result = f.auto_place(opponent, boat_list)
            opponent = result[0]
            opponent_boats = result[1]
            playing = True
    elif win == -1:
        # Game over, you lost
        win = input("VOUS AVEZ LOSE take the L \nLOSERRRRRS\n REJOUER ? ")
        if win == "OUI":
            start()
        else:
            running = False
    else:
        # Game over, you won
        win = input("vous avez win..... tryhard \n REJOUER ? ")
        if win == "OUI":
            start()
        else:
            running = False

            """
            resultat_ia = f.attack(opponent_attack_table, player, player_boats, True,
                                   f.ai(boat_list, opponent_attack_table))
            if resultat_ia == "Coulé":
                test = True
                for boat in player_boats:
                    for coords in boat:
                        if player[coords[0]][coords[1]] != ["Coulé", True]:
                            test = False
                            print(resultat_ia)
                            break
                if test:
                    win = -1
            print("Votre table")
            print(f.show_table(player))
            """
