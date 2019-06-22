package com.fossgalaxy.games.fireworks.ai.plgentmcts.worlds;

import com.fossgalaxy.games.fireworks.ai.iggi.Utils;
import com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode;
import com.fossgalaxy.games.fireworks.ai.plgentmcts.expconst.MCTSExpConst;
import com.fossgalaxy.games.fireworks.ai.plgentmcts.IterationObject;
import com.fossgalaxy.games.fireworks.ai.rule.logic.DeckUtils;
import com.fossgalaxy.games.fireworks.annotations.AgentBuilderStatic;
import com.fossgalaxy.games.fireworks.annotations.AgentConstructor;
import com.fossgalaxy.games.fireworks.state.Card;
import com.fossgalaxy.games.fireworks.state.Deck;
import com.fossgalaxy.games.fireworks.state.GameState;
import com.fossgalaxy.games.fireworks.state.Hand;
import com.fossgalaxy.games.fireworks.state.actions.Action;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.*;

/**
 * A version of MCTS that uses fixed iteration counts and limited determinations per world.
 *
 * Created by WebPigeon on 09/08/2016.
 */
public class MCTSWorlds extends MCTSExpConst {
    public static final int ITERATION_BUDGET = 10_000;
    public static final int DEFAULT_DET_COUNT = 40;

    protected final int iterationBudget;
    protected final int determinsations;

    protected final Random random;
    protected final Logger logger = LoggerFactory.getLogger(MCTSWorlds.class);

    private final boolean calcTree = false;

    /**
     * Create an MCTS agent which has the parameters.
     *
     * @param iterationBudget the total number of iterations to use
     * @param determinsations the number of worlds to explore
     * @param expConst the exploration constant to use for the search
     */
    @AgentConstructor("fixedMctsExpWorlds")
    public MCTSWorlds(int iterationBudget, int determinsations, double expConst) {
        super(expConst);
        this.iterationBudget = iterationBudget;
        this.determinsations = determinsations;

        this.random = new Random();
    }

    /**
     * Create an MCTS agent which has the parameters.
     *
     * @param iterationBudget the total number of iterations to use
     * @param determinsations the number of worlds to explore
     */
    public MCTSWorlds(int iterationBudget, int determinsations) {
        this(iterationBudget, determinsations, MCTSNode.DEFAULT_EXP_CONST);
    }

    @AgentBuilderStatic("MctsWorlds")
    public static MCTSWorlds buildMCTSDefaults(int iterationBudget, int determinsations) {
        return new MCTSWorlds(iterationBudget, determinsations, MCTSNode.DEFAULT_EXP_CONST);
    }

    @AgentBuilderStatic("MctsWorldsDefaults")
    public static MCTSWorlds buildMCTSDefaults() {
        return new MCTSWorlds(ITERATION_BUDGET, DEFAULT_DET_COUNT, MCTSNode.DEFAULT_EXP_CONST);
    }

    @Override
    public Action doMove(int agentID, GameState state) {

        MCTSNode root = new MCTSNode(
                (agentID + state.getPlayerCount() - 1) % state.getPlayerCount(),
                null,
                expConst,
                Utils.generateAllActions(agentID, state.getPlayerCount())
        );

        Map<Integer, List<Card>> possibleCards = DeckUtils.bindCard(agentID, state.getHand(agentID), state.getDeck().toList());
        List<Integer> bindOrder = DeckUtils.bindOrder(possibleCards);

        List<GameState> worlds = new ArrayList<>();
        for (int i=0; i < determinsations; i++) {
            GameState determinize = state.getCopy();
            Map<Integer, Card> myHandCards = DeckUtils.bindCards(bindOrder, possibleCards);

            Deck deck = determinize.getDeck();
            Hand myHand = determinize.getHand(agentID);
            for (int slot = 0; slot < myHand.getSize(); slot++) {
                Card hand = myHandCards.get(slot);
                myHand.bindCard(slot, hand);
                deck.remove(hand);
            }
            deck.shuffle();

            worlds.add(determinize);
        }

        int iterationsPerWorld = iterationBudget / determinsations;

        com.fossgalaxy.games.fireworks.ai.plgentmcts.IterationObject iterationObject = new IterationObject(agentID);
        for (int itr = 0; itr < iterationsPerWorld; itr++) {
            Collections.shuffle(worlds);

            for (GameState determinize : worlds) {
                GameState currentState = determinize.getCopy();

                MCTSNode current = select(root, currentState, iterationObject);
                int score = rollout(currentState, agentID, current);
                current.backup(score);
                if (calcTree) {
                    System.err.println(root.printD3());
                }
            }
        }


        return root.getBestNode().getAction();
    }

    @Override
    public String toString() {
        return "MCTSWorlds";
    }

}
