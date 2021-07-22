STATES = ["Hungry", "Thirsty", "Tired"]
ACTION = ["apple", "water", "sleep", "walk"]
TRANS  = [
    [ 2, 0, 0, 0 ],
    [ 1, 0, 1, 1 ],
    [ 2, 2, 1, 2 ],
]

state = 0

while True:
    print(f"You are {STATES[state]}")
    action = input("enter valid action: ")
    if not action in ACTION:
        print("Wrong Input")
        continue
    action = ACTION.index(action)
    state = TRANS[state][action]
