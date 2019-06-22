package com.fossgalaxy.games.fireworks.ai;

import com.fossgalaxy.games.fireworks.GameRunner;
import com.fossgalaxy.games.fireworks.GameStats;
import com.fossgalaxy.games.fireworks.ai.AgentPlayer;
import com.fossgalaxy.games.fireworks.players.Player;
import com.fossgalaxy.games.fireworks.utils.AgentUtils;
import com.fossgalaxy.stats.BasicStats;
import com.fossgalaxy.stats.StatsSummary;

import java.io.File;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Random;

/**
 * Game runner for testing.
 *
 * This will run a bunch of games with your agent so you can see how it does.
 */
public class App 
{
    public static void main( String[] args )
    {
        int numPlayers = 4;
        int numGames = 100;
        String agentName = "plgent"; // 2 for vanilla.

        Random random = new Random();
        StatsSummary statsSummary = new BasicStats();
        HashMap<String,Integer> barchart = new HashMap<String,Integer>();
        int count =0;
        barchart.put("Horrible",0);
        barchart.put("Poor",0);
        barchart.put("Honourable",0);
        barchart.put("Excellent",0);
        barchart.put("Extraordinary",0);
        barchart.put("Legendary",0);
        for (int i = 0; i < numGames; i++) {
            GameRunner runner = new GameRunner("test-game", numPlayers);

            //add your agents to the game
            for (int j = 0; j < numPlayers; j++) {
                // the player class keeps track of our state for us...
                Player player = new AgentPlayer(agentName, AgentUtils.buildAgent(agentName));
                runner.addPlayer(player);
            }

            GameStats stats = runner.playGame(random.nextLong());
            statsSummary.add(stats.score);
            if(stats.score>-1 & stats.score < 6)
            {
                barchart.put("Horrible",barchart.get("Horrible")+1);
            }
            else if(stats.score>5 & stats.score < 11)
            {
                barchart.put("Poor",barchart.get("Poor")+1);
            }
            else if(stats.score>10 & stats.score < 16)
            {
                barchart.put("Honourable",barchart.get("Honourable")+1);
            }
            else if(stats.score>15 & stats.score < 21)
            {
                barchart.put("Excellent",barchart.get("Excellent")+1);
            }
            else if(stats.score>20 & stats.score < 25)
            {
                barchart.put("Extraordinary",barchart.get("Extraordinary")+1);
            }
            else if(stats.score==25) {
                barchart.put("Legendary", barchart.get("Legendary") + 1);
            }
            ++count;
        }
        System.out.println("Horrible: " + barchart.get("Horrible"));
        System.out.println("Poor: " + barchart.get("Poor"));
        System.out.println("Honourable: " + barchart.get("Honourable"));
        System.out.println("Excellent: " + barchart.get("Excellent"));
        System.out.println("Extraordinary: " + barchart.get("Extraordinary"));
        System.out.println("Legendary: " + barchart.get("Legendary"));
        System.out.print("count " + count);
        //print out the stats
        System.out.println(String.format("Our agent: Avg: %f, min: %f, max: %f",
                statsSummary.getMean(),
                statsSummary.getMin(),
                statsSummary.getMax()));
    }
}
