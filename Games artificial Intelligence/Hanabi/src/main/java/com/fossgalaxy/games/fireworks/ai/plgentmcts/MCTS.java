package com.fossgalaxy.games.fireworks.ai.plgentmcts;

import com.fossgalaxy.games.fireworks.ai.Agent;
import com.fossgalaxy.games.fireworks.ai.iggi.Utils;
import com.fossgalaxy.games.fireworks.ai.rule.logic.DeckUtils;
import com.fossgalaxy.games.fireworks.annotations.AgentBuilderStatic;
import com.fossgalaxy.games.fireworks.annotations.AgentConstructor;
import com.fossgalaxy.games.fireworks.state.*;
import com.fossgalaxy.games.fireworks.state.actions.Action;
import com.fossgalaxy.games.fireworks.state.actions.PlayCard;
import com.fossgalaxy.games.fireworks.state.actions.TellColour;
import com.fossgalaxy.games.fireworks.state.actions.TellValue;
import com.fossgalaxy.games.fireworks.state.events.CardDrawn;
import com.fossgalaxy.games.fireworks.state.events.CardPlayed;
import com.fossgalaxy.games.fireworks.state.events.GameEvent;
import com.fossgalaxy.games.fireworks.utils.DebugUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.*;

/**
 * Created by WebPigeon on 09/08/2016.
 */
public class MCTS implements Agent {
    public static final int DEFAULT_ITERATIONS = 50_000;
    public static final int DEFAULT_ROLLOUT_DEPTH = 18;
    public static final int DEFAULT_TREE_DEPTH_MUL = 1;
    public static final int NO_LIMIT = 100;

    protected final int roundLength;
    protected final int rolloutDepth;
    protected final int treeDepthMul;
    protected final Random random;
    protected final Logger logger = LoggerFactory.getLogger(MCTS.class);

    private final boolean calcTree = false;

    /**
     * Create a default MCTS implementation.
     * <p>
     * This creates an MCTS agent that has a default roll-out length of 50_000 iterations, a depth of 18 and a tree
     * multiplier of 1.
     */
    public MCTS() {
        this(DEFAULT_ITERATIONS, DEFAULT_ROLLOUT_DEPTH, DEFAULT_TREE_DEPTH_MUL);
    }

    public MCTS(int roundLength) {
        this(roundLength, DEFAULT_ROLLOUT_DEPTH, DEFAULT_TREE_DEPTH_MUL);
    }

    @AgentConstructor("mcts")
    public MCTS(int roundLength, int rolloutDepth, int treeDepthMul) {
        this.roundLength = roundLength;
        this.rolloutDepth = rolloutDepth;
        this.treeDepthMul = treeDepthMul;
        this.random = new Random();
    }

    @AgentBuilderStatic("plgent")
    public static MCTS buildMCTSND() {
        return new MCTS(MCTS.DEFAULT_ITERATIONS, MCTS.NO_LIMIT, MCTS.NO_LIMIT);
    }

    @Override
    public Action doMove(int agentID, GameState state) {
        long finishTime = System.currentTimeMillis() + 1000;
        com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode root = new com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode(
                (agentID + state.getPlayerCount() - 1) % state.getPlayerCount(),
                null,
                Utils.generateAllActions(agentID, state.getPlayerCount())
        );

        Map<Integer, List<Card>> possibleCards = DeckUtils.bindCard(agentID, state.getHand(agentID), state.getDeck().toList());
        List<Integer> bindOrder = DeckUtils.bindOrder(possibleCards);


        if (logger.isTraceEnabled()) {
            logger.trace("Possible bindings: ");
            possibleCards.forEach((slot, cards) -> logger.trace("\t {} {}", slot, DebugUtils.getHistStr(DebugUtils.histogram(cards))));

            // Guaranteed cards
            logger.trace("Guaranteed Cards");

            possibleCards.entrySet().stream()
                    .filter(x -> x.getValue().size() == 1)
                    .forEach(this::printCard);

            logger.trace("We know the value of these");
            possibleCards.entrySet().stream()
                    .filter(x -> x.getValue().stream().allMatch(y -> y.value.equals(x.getValue().get(0).value)))
                    .forEach(this::printCard);

            DebugUtils.printTable(logger, state);
        }

//        for (int round = 0; round < roundLength; round++) {
        while(System.currentTimeMillis() < finishTime){
            //find a leaf node
            GameState currentState = state.getCopy();
            IterationObject iterationObject = new IterationObject(agentID);

            Map<Integer, Card> myHandCards = DeckUtils.bindCards(bindOrder, possibleCards);

            Deck deck = currentState.getDeck();
            Hand myHand = currentState.getHand(agentID);
            for (int slot = 0; slot < myHand.getSize(); slot++) {
                Card hand = myHandCards.get(slot);
                myHand.bindCard(slot, hand);
                deck.remove(hand);
            }
            deck.shuffle();

            com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode current = select(root, currentState, iterationObject);
            int score = rollout(currentState, agentID, current);
            current.backup(score);
            if(calcTree){
                System.err.println(root.printD3());
            }
        }

        if (logger.isInfoEnabled()) {
            for (com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode level1 : root.getChildren()) {
                logger.info("rollout {} moves: max: {}, min: {}, avg: {}, N: {} ", level1.getAction(), level1.rolloutMoves.getMax(), level1.rolloutMoves.getMin(), level1.rolloutMoves.getMean(), level1.rolloutMoves.getN());
                logger.info("rollout {} scores: max: {}, min: {}, avg: {}, N: {} ", level1.getAction(), level1.rolloutScores.getMax(), level1.rolloutScores.getMin(), level1.rolloutScores.getMean(), level1.rolloutScores.getN());
            }
        }

        if (logger.isTraceEnabled()) {
            logger.trace("next player's moves considerations: ");
            for (com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode level1 : root.getChildren()) {
                logger.trace("{}'s children", level1.getAction());
                level1.printChildren();
            }
        }

        Action chosenOne = root.getBestNode().getAction();
        if (logger.isTraceEnabled()) {
            logger.trace("Move Chosen by {} was {}", agentID, chosenOne);
            root.printChildren();
        }
        return chosenOne;
    }

    protected com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode select(com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode root, GameState state, IterationObject iterationObject) {
        com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode current = root;
        int treeDepth = calculateTreeDepthLimit(state);
        while (!state.isGameOver() && current.getDepth() < treeDepth) {
            com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode next;
            if (current.fullyExpanded(state)) {
                next = current.getUCTNode(state);
            } else {
                next = expand(current, state);
                return next;
            }
            if (next == null) {
                //XXX if all follow on states explored so far are null, we are now a leaf node
                return current;
            }
            current = next;

            int agent = current.getAgent();
            int lives = state.getLives();
            int score = state.getScore();

            Action action = current.getAction();
            if (action != null) {
                List<GameEvent> events = action.apply(agent, state);
                events.forEach(state::addEvent);
                state.tick();
            }

            if (iterationObject.isMyGo(agent)) {
                if (state.getLives() < lives) {
                    iterationObject.incrementLivesLostMyGo();
                }
                if (state.getScore() > score) {
                    iterationObject.incrementPointsGainedMyGo();
                }
            }
        }
        return current;
    }

    protected int calculateTreeDepthLimit(GameState state){
        return (state.getPlayerCount() * treeDepthMul) + 1;
    }

    /**
     * Select a new action for the expansion node.
     *
     * @param state   the game state to travel from
     * @param agentID the AgentID to use for action selection
     * @param node    the Node to use for expansion
     * @return the next action to be added to the tree from this state.
     */
    protected Action selectActionForExpand(GameState state, com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode node, int agentID) {
        Collection<Action> legalActions = node.getLegalMoves(state, agentID);
        if (legalActions.isEmpty()) {
            return null;
        }

        Iterator<Action> actionItr = legalActions.iterator();

        int selected = random.nextInt(legalActions.size());
        Action curr = actionItr.next();
        for (int i = 0; i < selected; i++) {
            curr = actionItr.next();
        }

        return curr;
    }

    protected com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode expand(com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode parent, GameState state) {
        int nextAgentID = (parent.getAgent() + 1) % state.getPlayerCount();
        Action action = selectActionForExpand(state, parent, nextAgentID);
        // It is possible it wasn't allowed
        if (action == null) {
            return parent;
        }
        if (parent.containsChild(action)) {
            // return the correct node instead
            return parent.getChild(action);
        }
        //XXX we may expand a node which we already visited? :S
        com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode child = new com.fossgalaxy.games.fireworks.ai.plgentmcts.MCTSNode(parent, nextAgentID, action, Utils.generateAllActions(nextAgentID, state.getPlayerCount()));
        parent.addChild(child);
        return child;
    }

    enum COLOURS
    {
        GREEN,WHITE,BLUE,ORANGE,RED,NULL
    }
    private COLOURS getColor(String tableString)
    {
        if(tableString.contains("GREEN"))
        {
            return COLOURS.GREEN;
        }
        else if(tableString.contains("WHITE"))
        {
            return COLOURS.WHITE;
        }
        else if(tableString.contains("BLUE"))
        {
            return COLOURS.BLUE;
        }
        else if(tableString.contains("ORANGE"))
        {
            return COLOURS.ORANGE;
        }
        else if(tableString.contains("RED"))
        {
            return COLOURS.RED;
        }
       return COLOURS.NULL;
    }

    private Action getCardToAction(List<Action> legalActions,int index)
    {
        for(Action a : legalActions)
        {
            if(a.toString().contains("play "+index))
                return a;
        }
        return legalActions.get(0);
    }
        // returns action to be used for rollouts.
    protected Action selectActionForRollout(GameState state, int playerID)
    {
        Collection<Action> legalActions = Utils.generateActions(playerID, state);

        List<Action> listAction = new ArrayList<>(legalActions);

        ArrayList<CardPlayed> cardPlayed = new ArrayList<CardPlayed>();
        // if table is empty then play a card.
        // get table.
        for(int i=0;i<state.getHistory().size();++i)
        {
            if(state.getHistory().get(i) instanceof CardPlayed)
            {
                cardPlayed.add((CardPlayed) state.getHistory().get(i));
            }
        }
        if(!cardPlayed.isEmpty()) {
            for (CardPlayed cp : cardPlayed) {
                for (int index = 0; index < state.getHand(playerID).getSize(); ++index) {
                    if (state.getHand(playerID).getCard(index).colour == CardColour.GREEN & getColor(cp.toString()) == COLOURS.GREEN) {
                        return getCardToAction(listAction, index);
                    }
                    if (state.getHand(playerID).getCard(index).colour == CardColour.BLUE & getColor(cp.toString()) == COLOURS.BLUE) {
                        return getCardToAction(listAction, index);
                    }
                    if (state.getHand(playerID).getCard(index).colour == CardColour.RED & getColor(cp.toString()) == COLOURS.RED) {
                        return getCardToAction(listAction, index);
                    }
                    if (state.getHand(playerID).getCard(index).colour == CardColour.ORANGE & getColor(cp.toString()) == COLOURS.ORANGE) {
                        return getCardToAction(listAction, index);
                    }
                    if (state.getHand(playerID).getCard(index).colour == CardColour.WHITE & getColor(cp.toString()) == COLOURS.WHITE) {
                        return getCardToAction(listAction, index);
                    }
                }
            }
        }
        if(state.getInfomation()==0)
        {
            // if no information start by discarding a card.
            for(Action a : listAction)
            {

            }
        }
            // then discard.
        // tell player with same colour or value as that on the table.
        Collections.shuffle(listAction);
        return listAction.get(0);
    }

    // rollout.
    protected int rollout(GameState state, final int agentID, MCTSNode current) {

        int playerID = agentID;
        int moves = 0;
        while (!state.isGameOver() && moves < rolloutDepth)
        {
            Action action = selectActionForRollout(state, playerID);
            List<GameEvent> events = action.apply(playerID, state);
            events.forEach(state::addEvent);
            state.tick();
            playerID = (playerID + 1) % state.getPlayerCount();
            moves++;
        }

        current.backupRollout(moves, state.getScore());
        return state.getScore();
    }

    @Override
    public String toString() {
        return "MCTS";
    }

    private void printCard(Map.Entry<Integer, List<Card>> entry) {
        logger.trace("{} : {}", entry.getKey(), entry.getValue());
    }

}
