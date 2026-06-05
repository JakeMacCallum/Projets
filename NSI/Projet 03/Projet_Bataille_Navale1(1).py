# -*- coding: utf-8 -*-
"""
Created on Sun Mar 24 20:21:46 2024

@author: Paul
"""

import random

"""plateau =[
    [0,0,0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0,0,0]]
"""
largeur = 10
hauteur = 10


class bataille_navale:
    def __init__(self, nom):
        self.plateau = [
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]]
        self.bateau = [2, 3, 3, 4, 5]
        self.nom = nom
        self.vie = 2

    def attaquer(self, coord1, coord2, plateau_adversair):
        if plateau_adversair[coord1][coord2] == 5:
            plateau_adversair[coord1][coord2] = "T"
            print(self.nom, "a touché un navire sur la ligne : ", coord1 + 1, "et sur la colonne : ", coord2 + 1)
            if self.nom == "j1":
                j2.vie -= 1
            elif self.nom == "j2":
                j1.vie -= 1

        else:
            plateau_adversair[coord1][coord2] = "R"
            print(self.nom, "a raté son tir")
            jouer = False
            return jouer

    def placer(self, plateau):
        for i in range(len(self.bateau)):
            taille = self.bateau[i]
            bataille_navale.afficher(self, self.plateau)
            print()

            # partie joueur de la fonction
            if self.nom == "j1":
                print("tu vas placer un bateau de taille : ", self.bateau[i])
                sens = input("Donne le sens du bateau (H ou V) : ")
                while sens != "H" and sens != "V":
                    sens = input("Le sens donné n'est pas bon, recommence, donne le sens du bateau (H ou V) : ")

                if sens == "H":
                    coord1 = int(input("Donne la ligne de ton bateau : ")) - 1
                    coord2 = int(input("Donne la colonne du coté gauche de ton bateau : ")) - 1

                    while bataille_navale.verif_ecart_bateau(j1, sens, self.bateau[i], coord1, coord2) == False:
                        print("Le bateau que tu veux placer entre en collision il faut le changer de place : ")

                        sens = input("Donne le sens du bateau (H ou V) : ")
                        if sens == "H":
                            coord1 = int(input("Donne la ligne de ton bateau : ")) - 1
                            coord2 = int(input("Donne la colonne du coté gauche de ton bateau : ")) - 1

                        elif sens == "V":
                            coord2 = int(input("Donne la colonne de ton bateau : ")) - 1
                            coord1 = int(input("Donne la ligne du coté le plus haut de ton bateau : ")) - 1

                    for i in range(taille):
                        plateau[coord1][coord2 + i] = 5


                elif sens == "V":
                    coord2 = int(input("Donne la colonne de ton bateau : ")) - 1
                    coord1 = int(input("Donne la ligne du coté le plus haut de ton bateau : ")) - 1

                    while bataille_navale.verif_ecart_bateau(j1, sens, self.bateau[i], coord1, coord2) == False:
                        print("Le bateau que tu veux placer entre en collision il faut le changer de place : ")

                        sens = input("Donne le sens du bateau (H ou V) : ")
                        if sens == "H":
                            coord1 = int(input("Donne la ligne de ton bateau : ")) - 1
                            coord2 = int(input("Donne la colonne du coté gauche de ton bateau : ")) - 1

                        elif sens == "V":
                            coord2 = int(input("Donne la colonne de ton bateau : ")) - 1
                            coord1 = int(input("Donne la ligne du coté le plus haut de ton bateau : ")) - 1

                    for i in range(taille):
                        plateau[coord1 + i][coord2] = 5



            # partie IA de la fonction
            else:
                sens = random.randint(0, 1)

                if sens == 0:  # Horizontale

                    coord1 = random.randint(0, 9)
                    coord2 = random.randint(0, 9)
                    res = bataille_navale.verif_ecart_bateau(j2, sens, self.bateau[i], coord1, coord2)
                    print('je suis à la sortie res = ', res)
                    while res == False:
                        print('je suis dans la boucle while')
                        sens = random.randint(0, 1)
                        coord1 = random.randint(0, 9)
                        coord2 = random.randint(0, 9)
                        res = bataille_navale.verif_ecart_bateau(j2, sens, self.bateau[i], coord1, coord2)
                        print("H sortie", "taille:", taille, "coord1:", coord1, "coord2:", coord2, 'verif: ', res)

                    print("Je vais poser la merde de bateau")
                    print("IA 1 : ", "taille: ", taille, "coord1:", coord1, "coord2:", coord2)
                    for i in range(taille):
                        plateau[coord1][coord2 + i] = 5


                elif sens == 1:  # Verticale
                    coord1 = random.randint(0, 9)
                    coord2 = random.randint(0, 9)
                    print('je suis à la sortie res = ', res)
                    res = bataille_navale.verif_ecart_bateau(j2, sens, self.bateau[i], coord1, coord2)
                    print('je suis à la sortie res = ', res)

                    while res == False:
                        print('je suis dans la boucle while')
                        sens = random.randint(0, 1)
                        coord1 = random.randint(0, 9)
                        coord2 = random.randint(0, 9)
                        res = bataille_navale.verif_ecart_bateau(j2, sens, self.bateau[i], coord1, coord2)
                        print("V sortie", "taille:", taille, "coord1:", coord1, "coord2:", coord2, 'verif: ', res)

                    print("Je vais poser la merde de bateau")
                    print("IA 2 : ", "taille: ", taille, "coord1:", coord1, "coord2:", coord2)
                    for i in range(taille):
                        plateau[coord1 + i][coord2] = 5

    def verif_ecart_bateau(self, orientation, taille, coord1, coord2):
        verif = True

        if self.nom == "j1":
            if orientation == "H":

                while taille + coord2 > 10:
                    print("La colonne du coté gauche de ton bateau est trop proche du coté droit")
                    coord2 = int(input(
                        "Donne une colonne du coté gauche de ton bateau plus à gauche (un nombre plus petit) : ")) - 1

                for i in range(taille):
                    if self.plateau[coord1][coord2 + i] == 5:
                        verif = False
                    if coord2 - 1 >= 0:
                        if self.plateau[coord1][coord2 - 1] == 5:
                            verif = False
                    if coord1 - 1 >= 0:
                        if self.plateau[coord1 - 1][coord2 + i] == 5:
                            verif = False
                    if coord1 + 1 < 10:
                        if self.plateau[coord1 + 1][coord2 + i] == 5:
                            verif = False
                return verif

            elif orientation == "V":

                while taille + coord1 > 10:
                    print("La ligne du coté le plus haut de ton bateau est trop proche du sol")
                    coord1 = int(input(
                        "Donne une ligne du coté le plus haut de ton bateau plus en haut (un nombre plus petit) : ")) - 1

                for i in range(taille):
                    if self.plateau[coord1 + i][coord2] == 5:
                        verif = False
                    if coord1 - 1 >= 0:
                        if self.plateau[coord1 - 1][coord2] == 5:
                            verif = False
                    if coord2 - 1 >= 0:
                        if self.plateau[coord1 + i][coord2 - 1] == 5:
                            verif = False
                    if coord2 + 1 < 10:
                        if self.plateau[coord1 + i][coord2 + 1] == 5:
                            verif = False
                return verif

        # IA
        elif self.nom == "j2":
            if orientation == 0:

                while taille + coord2 > 10:
                    coord2 = random.randint(0, 9)
                for i in range(taille):
                    if self.plateau[coord1][coord2 + i] == 5:
                        verif = False
                    if coord2 - 1 >= 0:
                        if self.plateau[coord1][coord2 - 1] == 5:
                            verif = False
                    if coord1 - 1 >= 0:
                        if self.plateau[coord1 - 1][coord2 + i] == 5:
                            verif = False
                    if coord1 + 1 < 10:
                        if self.plateau[coord1 + 1][coord2 + i] == 5:
                            verif = False

                if (verif == True):
                    print("H sortie", "taille:", taille, "coord1:", coord1, "coord2:", coord2)
                return verif

            elif orientation == 1:

                while taille + coord1 > 10:
                    coord1 = random.randint(0, 9)
                for i in range(taille):
                    if self.plateau[coord1 + i][coord2] == 5:
                        verif = False
                    if coord1 - 1 >= 0:
                        if self.plateau[coord1 - 1][coord2] == 5:
                            verif = False
                    if coord2 - 1 >= 0:
                        if self.plateau[coord1 + i][coord2 - 1] == 5:
                            verif = False
                    if coord2 + 1 < 10:
                        if self.plateau[coord1 + i][coord2 + 1] == 5:
                            verif = False
                if (verif == True):
                    print("V sortie", "taille:", taille, "coord1:", coord1, "coord2:", coord2)
                return verif

    def afficher(self, plateau):
        print()
        for i in range(0, len(plateau)):
            print(plateau[i])

    def action(self, qui_joue):
        if qui_joue == "joueur":
            coord1 = int(input("Donne la ligne oû tu veux attaquer : ")) - 1
            coord2 = int(input("Donne la colonne oû tu veux attaquer : ")) - 1
            j1.attaquer(coord1, coord2, j2.plateau)

        elif qui_joue == "IA":
            coord1 = random.randint(0, 9)
            coord2 = random.randint(0, 9)
            j2.attaquer(coord1, coord2, j1.plateau)

    def tour_de_jeu(j1, j2):

        # bataille_navale.placer(j1,j1.plateau)

        bataille_navale.placer(j2, j2.plateau)
        """
        while j1.vie>0 and j2.vie>0:
            print()
            print("plateau du joueur 1 : ")
            bataille_navale.afficher(j1,j1.plateau) 
            print("plateau du joueur 2 : ")
            bataille_navale.afficher(j2,j2.plateau) 
            
            #bataille_navale.action( j1,"joueur")
            bataille_navale.action( j2,"IA")
        """
        if j1.vie == 0 and j2.vie == 0:
            print("Il y a égalité")
        elif j1.vie >= 0:
            print("Le joueur 1 a gagné")
        elif j2.vie >= 0:
            print("Le joueur 2 a gagné")


j1 = bataille_navale("j1")
j2 = bataille_navale("j2")
bataille_navale.tour_de_jeu(j1, j2)

"""            
    #TK inter
    def traceLignes():
        
        for i in range(hauteur+1):
            #lignes horizontales
            can.create_line (subx, suby+i*suby, cote_largeur, suby+i*suby, fill='black')
            #texte ABCD
            can.create_text (subx/2, suby*3/2+i*suby, text = chr(i+65), font = "Arial 12", fill='black')
        for i in range(largeur+2):
            #lignes verticales
            can.create_line (subx+i*subx,suby, subx+i*subx,cote_hauteur, fill='black')
            #texte 0123
            if i <10 :can.create_text (subx*3/2+i*subx, suby/2, text = chr(i+48), font = "Arial 12", fill='black')
            #texte 10 11 12 ..., si besoin
            else :can.create_text (subx*3/2+i*subx, suby/2, text = "1"+chr(i+48-10), font = "Arial 12", fill='red')
    
            

----- debut de tk
           
            
            
fen=Tk()
fen.title('Titre')
if largeur > hauteur:
    cote_largeur = 770                                #largeur/hauteur max de la fenetre
    cote_hauteur = int(cote_largeur*hauteur/largeur)  #largeur/hauteur adaptative
else:
    cote_hauteur = 770                                #largeur/hauteur max de la fenetre
    cote_largeur = int(cote_hauteur*largeur/hauteur)  #largeur/hauteur adaptative

subx = cote_largeur/(largeur+1)                       #largeur des cases
suby = cote_hauteur/(hauteur+1)                       #hauteur des cases
rayonRond = (min(subx,suby))/4                        #un rond ni trop petit ni trop grand
couleurs = ['white','red','dark gray']                #liste des couleurs des ronds
can=Canvas(fen, bg=couleurs[2], height=cote_hauteur, width=cote_largeur)
can.pack() 
                                           #trace tout
                              #affichage du plateau en mode texte
bataille_navale.traceLignes()

#tk : surveille la souris et lance la fonction loop
                        #clic gauche qui lance la fonction loop
#fen.bind("<Button-3>",loop)                          #clic droit

fen.mainloop()


----- fin de tk

"""
