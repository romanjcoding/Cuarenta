# Cuarenta
## Background

## Bot Design
Cuarenta is a turn-based card game with short rounds between deck refills and scoring. For any two fixed hands, we can calculate an upper bound on possible game states: $\sum_{t=0}^{10} \binom{5}{\lceil t/2 \rceil} \binom{5}{\lceil t/2 \rceil}3^t = 405,733$. Since this is computationally trivial, Perfect Information Monte Carlo (PIMC) provides an effective starting point for building a bot, though this approach has some limitations discussed later.

### Perfect Information Monte Carlo
The bot maintains estimated probabilities for each card rank the opponent might hold. This estimation uses two components:
1. `count`: The number of each rank remaining in the deck, adjusted for cards in the bot's hand (which the opponent cannot have) or previously played cards, and updated only when the deck is reshuffled.
2. `probability_weight`: A heuristic-based modifier updated whenever hands are refreshed, using information from opponent moves, particularly when the opponent doesn't make certain plays like caidas (captures) or limpias (clears).

The PIMC algorithm runs in three steps:

1. Sample opponent hand: Draw 5 cards without replacement from a weighted distribution, where each rank's weight equals `count × probability_weight`.
2. Evaluate moves: Use a negamax algorithm to determine the value of playing each card in the bot's hand optimally, given the sampled opponent hand.
3. Aggregate results: Repeat for N iterations and calculate the expected value and variance for each possible play.

Since estimated hands are i.i.d. with finite leaf evaluation, i.e., finite variance, we can apply the Central Limit Theorem and derive confidence intervals. This provides a criterion for determining when we can be confident in selecting a particular move. While many factors may influence this confidence, a simple and especially relevant metric is the hand size of each player, as it directly affects the variance and reliability of the estimates.

Each Monte-Carlo rollout is a Bernoulli outcome indicating wether the selected move can be distinguished within a given $\epsilon$. For a fixed matchup and iteration count $N$, the number of successes is $X \sim \text{Binomial}(N, \theta)$ for an unknown paramater $\theta$. We use a uniform prior $\theta \sim \text{Beta}(1,1)$ to generate credible intervalss.

![confidence](https://github.com/user-attachments/assets/7dde131e-b177-436e-a741-f9a09480d413)

In particular, for a 95% chance of being able to distinguish moves with expected value a maximum of $\epsilon = 0.25$ apart, we choose $N = 50000, 5000, 10000, 1000, 2500, 500, 250, 100, 0$ for their respective hand sizes. This is used as the bot's default iteration count, unless overridden by time constraints.

### Estimating Heuristics
An important consideration is that the negamax search terminates in an imperfect state. Two factors remain unaccounted for: the number of captured cards, we  and the cards on the table. Estimating their impact in a single pass is challenging, but we can bootstrap heuristics through repeated self-play. By running Bot v. Bot multiple times, we can derive a rough estimate of the expected change in score based on the number of captured cards. Once this heuristic is established, we can iteratively refine it by having the bot play against itself using the updated heuristics, gradually improving our evaluation over successive iterations.

![distribution](https://github.com/user-attachments/assets/e3c2a599-dbaa-47d9-984c-959b04d9d383)


### Strategy Fusion
One of the primary issues with PIMC is strategy fusion. Consider the simple example of a modified game of Rock, Paper, Scissors.
<p align="center">
<img width="484" height="312" alt="Screenshot 2026-01-15 at 10 22 36 PM" src="https://github.com/user-attachments/assets/cd0d67fa-4777-43eb-bea1-835385eab6b8" />
</p>
