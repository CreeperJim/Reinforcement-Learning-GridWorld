import itertools
from matplotlib import pyplot as plt

plt.style.use('ggplot')
import numpy as np
import sys

import warnings
warnings.simplefilter("ignore", DeprecationWarning)

import groverIteration as GI
from qiskit import (
    QuantumCircuit,
    execute,
    Aer,
)

if "../" not in sys.path:
    sys.path.append("../")

from collections import defaultdict
from lib import plotting
from tqdm import tqdm


class GridWorldEnv:
    
    def __init__(self):
        self.grid = [[1, 2, 3, 4, 5],
                     [6, 7, 8, 9, 10],
                     [11, 12, -1, 13, 14],
                     [15, 16, -1, 17, 18],
                     [19, 20, 21, 22, 23]]
        self.state = [0, 0]
        self.position = 1
        self.actions = [0, 1, 2, 3]  # left, up, right, down
        self.states = 23
        self.final_state = 23
        self.reward = [[0, 0, 0, 0, 0],
                       [0, 0, 0, 0, 0],
                       [0, 0, 0, 0, 0],
                       [0, 0, 0, 0, 0],
                       [0, 0, -10, 0, 10]]
        self.done = False

    def reset(self):
        self.state = [0, 0]
        self.position = 1
        self.done = False

        return self.position

    def give_MDP_info(self):
        return self.states, len(self.actions), self.final_state

    def step(self, action):

        # chance = uniform(0, 1)
        # if chance < 0.05:
        #	action = action - 1
        # elif chance < 0.10:
        #	action = action + 1
        # elif chance < 0.20:
        #	action = -10 # stay, no action

        if action == 0:
            self.state[1] -= 1
        elif action == 1:
            self.state[0] -= 1
        elif action == 2:
            self.state[1] += 1
        elif action == -1 or action == 3:
            self.state[0] += 1

        if self.state[0] < 0:
            self.state[0] = 0
        elif self.state[0] > 4:
            self.state[0] = 4
        elif self.state[1] < 0:
            self.state[1] = 0
        elif self.state[1] > 4:
            self.state[1] = 4
        elif self.state[1] == 2 and (self.state[0] == 2 or self.state[0] == 3):
            if action == 0:
                self.state[1] = 3
            elif action == 1:
                self.state[0] = 4
            elif action == 2:
                self.state[1] = 1
            elif action == -1 or action == 3:
                self.state[0] = 1

        self.position = self.grid[self.state[0]][self.state[1]]
        reward = self.reward[self.state[0]][self.state[1]]

        if self.position == self.final_state:
            self.done = True
        return self.position, reward, self.done


def groverIteration(eigen_action, action, reward, next_state_value):
    # if L < 2:
    L = int(0.2 * (reward + next_state_value))  # reward + value of the next_state, k is .3 which is arbitrary
    if L > 1:
        L = 1

    qr = eigen_action.qubits
    if action == 0:
        for x in range(L):
            eigen_action, qr = GI.gIteration00(eigen_action, qr)
    elif action == 1:
        for x in range(L):
            eigen_action, qr = GI.gIteration01(eigen_action, qr)
    elif action == 2:
        for x in range(L):
            eigen_action, qr = GI.gIteration10(eigen_action, qr)
    elif action == 3:
        for x in range(L):
            eigen_action, qr = GI.gIteration11(eigen_action, qr)

    return eigen_action


def remember(eigen_state, action, state_value, next_state_value, reward, done):
    memory[eigen_state].append([action, state_value, next_state_value, reward, done])
    # determines the action to make, collapses/measures the eigen_action into a move to make


def collapse_action_select_method(circuit):
    qr = circuit.qubits
    cr = circuit.clbits
    circuit.measure(qr, cr)
    backend = Aer.get_backend('qasm_simulator')
    result = execute(circuit, backend=backend, shots=1).result()

    # convert result to int
    classical_state = int(list(result.get_counts().keys())[0], base=2)
    return classical_state


def q_learning(env, num_episodes, discount_factor=0.9, alpha=0.8):  # , epsilon=0.1):
    """
    Q-Learning algorithm: Off-policy TD control. Finds the optimal greedy policy
    while following an epsilon-greedy policy
    
    Args:
        env: OpenAI environment.
        num_episodes: Number of episodes to run for.
        discount_factor: Gamma discount factor.
        alpha: TD learning rate.
        epsilon: Chance the sample a random action. Float between 0 and 1.
    
    Returns:
        A tuple (Q, episode_lengths).
        Q is the optimal action-value function, a dictionary mapping state -> action values.
        stats is an EpisodeStats object with two numpy arrays for episode_lengths and episode_rewards.
    """

    # The final action-value function.
    # A nested dictionary that maps state -> (action -> action-value).
    Q = defaultdict(lambda: np.zeros(env.action_space.n))
    memory = defaultdict(tuple)

    # Keeps track of useful statistics
    stats = plotting.EpisodeStats(
        episode_lengths=np.zeros(num_episodes),
        episode_rewards=np.zeros(num_episodes))

    # The policy we're following
    # policy = make_epsilon_greedy_policy(Q, epsilon, env.action_space.n)

    for i_episode in tqdm(range(num_episodes)):
        if (i_episode + 1) % 100 == 0:
            print("\rEpisode {}/{}.".format(i_episode + 1, num_episodes), end="")

        # Reset the environment and pick the first action
        eigen_state = env.reset()

        # One step in the environment
        # total_reward = 0.0
        for t in itertools.count():
            if eigen_state in memory:
                mem_list = memory[eigen_state]
                action = mem_list[0]
                state_value = mem_list[1]
                next_state = mem_list[2]

                if next_state in memory:
                    next_state_value = memory[next_state][1]
                else:
                    next_state_value = 0.0
                reward = mem_list[3]

                circuit = QuantumCircuit(2, 2)
                circuit.h(0)
                circuit.h(1)
                circuit = groverIteration(circuit, action, reward, next_state_value)
            else:
                # Prepare the n-qubit registers
                circuit = QuantumCircuit(2, 2)
                circuit.h(0)
                circuit.h(1)
                state_value = 0.0

            action = collapse_action_select_method(circuit)
            next_eigen_state, reward, done = env.step(action)

            if next_eigen_state in memory:
                mem_list = memory[next_eigen_state]
                next_state_value = mem_list[1]
            else:
                next_state_value = 0.0

            # Update state value
            state_value = state_value + alpha * (reward + (discount_factor * next_state_value) - state_value)
            # print(state_value)

            memory[eigen_state] = (action, state_value, next_eigen_state, reward)

            stats.episode_rewards[i_episode] += (discount_factor ** t) * reward
            stats.episode_lengths[i_episode] = t

            if done:
                break

            # state = next_state
            eigen_state = next_eigen_state

    return Q, stats, memory


if __name__ == '__main__':
    env = GridWorldEnv()
    Q, stats, memory = q_learning(env, 500)
    
    for state in memory:
        print(memory[state])
    
    plotting.plot_episode_stats(stats)
