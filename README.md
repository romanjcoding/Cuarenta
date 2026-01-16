# Cuarenta
## Background

## Bot Design
Cuarenta is a turn-based card game with short rounds between deck refills and scoring. For any two fixed hands, we can calculate an upper bound on possible game states: $\sum_{t=0}^{10} \binom{5}{\lceil t/2 \rceil} \binom{5}{\lceil t/2 \rceil}3^t = 405,733$. Since this is computationally trivial, Perfect Information Monte Carlo (PIMC) provides an effective starting point for building a bot, though this approach has some limitations discussed later.

### Perfect Information Monte Carlo
The bot maintains estimated probabilities for each card rank the opponent might hold. This estimation uses two components:
1. `count`: The number of each rank remaining in the deck, adjusted for cards in the bot's hand (which the opponent cannot have) or previously played cards, and updated only when the deck is reshuffled.
2. `probability_weight`: A heuristic-based modifier updated whenever hands are refreshed, using information from opponent moves, particularly when the opponent doesn't make certain plays like caidas (captures) or limpias (clears).

The PIMC algorithm runs in three steps:

1. Sample opponent hand: Draw 5 cards without replacement from a weighted distribution, where each rank's weight equals `count × probability_weight`. The distribution changes with each draw.
2. Evaluate moves: Use a negamax algorithm to determine the value of playing each card in the bot's hand optimally, given the sampled opponent hand.
3. Aggregate results: Repeat for N iterations and calculate the expected value and variance for each possible play.

### Estimating Heuristics
An important consideration is that the negamax search terminates in an imperfect state. Two factors remain unaccounted for: the number of captured cards and the cards on the table [WIP, possibly with neural networks]. Estimating their impact in a single pass is challenging, but we can bootstrap heuristics through repeated self-play. By running Bot v. Bot multiple times, we can derive a rough estimate of the expected change in score based on the number of captured cards. Once this heuristic is established, we can iteratively refine it by having the bot play against itself using the updated heuristics, gradually improving our evaluation over successive iterations.

<p align="center">
<img width="1200" height="800" alt="heuristics" src="https://github.com/user-attachments/assets/99bce393-e5f1-458e-a353-22227ab5fd08" />
</p>

We notice there is a constant portion and a nearly linear portion, so we approximate the data with a smooth extension of an ReLu, the softplus function. In addition, we shift and scale the function, fitting to $f(x) = b + a\log(1 + e^{k(x - h)})$.

<p align = "center">
<img width="1200" height="800" alt="heuristics_with_fits" src="https://github.com/user-attachments/assets/68b3ce63-894b-4cb1-bfa5-2ac4d0c8796b" />
</p>


### Strategy Fusion
One of the primary issues with PIMC is strategy fusion. Consider the simple example of a modified game of Rock, Paper, Scissors.
<p align="center">
<img width="484" height="312" alt="Screenshot 2026-01-15 at 10 22 36 PM" src="https://github.com/user-attachments/assets/cd0d67fa-4777-43eb-bea1-835385eab6b8" />
</p>
