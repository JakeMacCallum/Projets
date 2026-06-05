import requests
import random
import json

with open('types.json', 'r') as f:
    dictionnaire_types = json.load(f)
with open('attaques.json', 'r') as f:
    dictionnaire_attaques = json.load(f)


class Pokemon:
    def __init__(self, name=None):
        response = requests.get(f'https://pokeapi.co/api/v2/pokemon/{name}/').json()
        self.name = name
        self.identification = response['id']
        self.types = {}
        for t in response['types']:
            self.types[t['type']['name']] = dictionnaire_types[t['type']['name']]
        self.stats = {
            "hp": response['stats'][0]['base_stat'],
            'atk': response['stats'][1]['base_stat'],
            'defense': response['stats'][2]['base_stat'],
            'speed': response['stats'][5]['base_stat']
        }
        liste_attaques = [
            random.choice(list(self.types.keys())) + "Punch",
            random.choice(list(self.types.keys())) + "Kick",
            random.choice(list(self.types.keys())) + "Slam",
            random.choice(list(self.types.keys())) + "Destroy"
        ]
        self.moves = {}
        for move in liste_attaques:
            self.moves[move] = dictionnaire_attaques[move]

    def get_name(self):
        return self.name

    def get_identification(self):
        return self.identification

    def get_types(self):
        return self.types

    def get_stats(self):
        return self.stats

    def get_moves(self):
        return self.moves

    def set_name(self, name):
        self.name = name

    def set_identification(self, identification):
        self.identification = identification

    def set_types(self, types):
        self.types = types

    def set_stats(self, stats):
        self.stats = stats

    def set_moves(self, moves):
        self.moves = moves

    def receive_damage(self, amount):
        self.stats["hp"] -= amount

    def attack(self, move, other):
        if random.randint(1, 100) <= self.moves[move]['accuracy']:
            damage_multplier = 1
            for types in other.types.keys():
                if self.moves[move]['type'] in other.types[types]["no_damage_to"]:
                    return [f"{move} was ineffective"]
                if self.moves[move]['type'] in other.types[types]["double_damage_to"]:
                    damage_multplier *= 1.5
                if self.moves[move]['type'] in other.types[types]["half_damage_to"]:
                    damage_multplier *= 0.5
            other.stats["hp"] -= int((self.moves[move]['atk'] / other.stats['defense']) * self.stats['atk'] * (
                    random.randint(80, 100) / 100) * damage_multplier)
            if damage_multplier == 1 or damage_multplier == 0.75:
                return [f"{self.name} used {move}"]
            elif damage_multplier >= 1.5:
                return [f"{self.name} used {move}", "It was super effective"]
            elif damage_multplier <= 0.5:
                return [f"{self.name} used {move}", "It wasn't very effective"]
        else:
            return [f"{self.name} missed !"]
        return ["ok?"]

    def evaluate(self, other):
        move = random.choice(list(self.moves))
        return self.attack(move, other)
