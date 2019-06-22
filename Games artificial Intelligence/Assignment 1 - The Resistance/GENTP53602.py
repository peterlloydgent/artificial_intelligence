from player import Bot
import random

class GENTP53602(Bot):
    """Decision tree bot"""

    def select(self, players, count):
        self.__blackboard.setCount(count)
        self.__blackboard.setPlayers(players)
        self.__blackboard.setBot(self)
        self.__decisionTree.insertSelectionNode(self.__blackboard)
        self.__decisionTree.executeTreeSearch()
        return self.__blackboard.getSelectTeam()

    def vote(self, team):
        self.__blackboard.setTeam(team)
        self.__decisionTree.executeTreeSearch()
        return self.__blackboard.getVoting()


    def sabotage(self):
        self.__decisionTree.executeTreeSearch()
        return self.__blackboard.getSabotage()

    def onMissionComplete(self,sabotaged):
        if sabotaged>0:
            print("A mission was sabotaged." + str(sabotaged) + " times")
            print("Therefore the failed team has " + str(sabotaged) + " spies in it. The team is: " + str(self.__blackboard.getTeam()))
            team = self.__blackboard.getTeam()
            # in the rare case that all members of a team are spies
            if sabotaged == len(team):
                print("team is full to the brim with spies so all agents must be spies")
                positivelyIdentifiedSpies = set()
                for spy in self.__blackboard.getTeam():
                    positivelyIdentifiedSpies.add(spy)
                print("identified all spies: " + str(positivelyIdentifiedSpies))
                self.__blackboard.setIdentifiedSpies(positivelyIdentifiedSpies)
            else:
                suspects = set()
                for suspect in team:
                    suspects.add(suspect)
                if len(self.__blackboard.getSuspects())==0:
                    self.__blackboard.setSuspects(suspects)
                else:
                    #if intersection then set intersecting bot is identified as a spy after two mission failiures
                    identifiedSpies = self.__blackboard.getSuspects().intersection(suspects)
                    print("Identified potential spies: " + str(identifiedSpies))
                    compositeSpies = self.__blackboard.getIdentifiedSpies().union(identifiedSpies)
                    self.__blackboard.setIdentifiedSpies(compositeSpies)
                    ## check for outlier spies (spies not intersecting but hiding with other players

    def onGameRevealed(self, players, spies):
        self.__blackboard = Blackboard()
        self.__blackboard.setSpies(spies)
        self.__blackboard.setBot(self)
        self.__blackboard.setPlayers(players)
        self.__blackboard.setTeam([])
        self.__blackboard.setSuspects(set())
        self.__blackboard.setIdentifiedSpies(set())
        self.__decisionTree = self.__buildTree(self.__blackboard)

    def __buildTree(self, blackboard):
        root = DecisionTree(IsSpy(blackboard))
        sabotageSpy = Node(SabotageSpy(blackboard))
        spyVoting = Node(SpyVoting(blackboard))
        sabotageSpy.addRight(spyVoting)
        root.addYes(sabotageSpy)

        sabotageResistance = Node(SabotageResistance(blackboard))
        resistanceVoting = Node(ResistanceVoting(blackboard))
        sabotageResistance.addLeft(resistanceVoting)
        root.addNo(sabotageResistance)
        return root

class Blackboard(object):
    def getIdentifiedSpies(self):
        return self.__identifiedSpies
    def setIdentifiedSpies(self, identifiedSpies):
        self.__identifiedSpies = identifiedSpies
    def getSuspects(self):
        return self.__suspects;
    def setSuspects(self, suspects):
        self.__suspects = suspects
    def setBot(self,bot):
        self.__bot = bot
    def getBot(self):
        return self.__bot
    def setSelectTeam(self,selectTeam):
        self.__selectTeam = selectTeam
    def getSelectTeam(self):
        return self.__selectTeam
    def setPlayers(self, players):
        self.__players = players
    def getPlayers(self):
        return self.__players
    def setCount(self, count):
        self.__count = count
    def getCount(self):
        return self.__count
    def getVoting(self):
        return self.__voting
    def setVoting(self, vote):
        self.__voting = vote
    def setTeam(self, team):
        self.__team = team
    def getTeam(self):
        return self.__team
    def sabotage(self, value):
        self.__sabotage = value
    def getSabotage(self):
        return self.__sabotage
    def setSpies(self, spies):
        self.__spies = spies
    def getSpies(self):
        return self.__spies


class DecisionTree:
    def __init__(self, command, left=None, right=None):
        self.__node = Node(command, left, right)
        self.__selectionNodeAdded = False

    def executeTreeSearch(self):
        result = self.__node
        while result is not None:
            result = result.execute()
    def addYes(self,node):
        self.__node.addRight(node)
    def addNo(self,node):
        self.__node.addLeft(node)
    def insertSelectionNode(self, blackboard):
        if not self.__selectionNodeAdded:
            isSpy = self.__node.execute();
            if str(isSpy) == "SabotageSpy":
                root = self.__node
                root = root.execute()
                root = root.execute()
                #print("adding spyselection to " + str(root))
                root.addRight(Node(SpySelection(blackboard)))
            else:
                root = self.__node
                root = root.execute()
                root = root.execute()
                #print("adding resistance selection to " + str(root))
                root.addRight(Node(ResistanceSelection(blackboard)))
            self.__selectionNodeAdded = True

class Node:
    def __init__(self,command, left=None, right=None):
        self.left = left;
        self.right = right;
        self.command = command
    def execute(self):
        result = self.command.executeFunction()
        if result<0:#negative for no
            return self.left
        if result>0:#positive for yes
            return self.right
    def getleft(self):
        return self.left;
    def getRight(self):
        return self.right
    def addRight(self,node):
        self.right = node
    def addLeft(self,node):
        self.left = node
    def __str__(self):
        return str(self.command)

class CommandObject():
    def executeFunction(self):
        pass
class IsSpy(CommandObject):
    def __init__(self, blackboard):
        self.__blackboard = blackboard

    def executeFunction(self):
        if len(self.__blackboard.getSpies())>0:
            return 1
        else:
            return -1
    def __str__(self):
        return "IsSpy"

class SabotageSpy(CommandObject):
    def __init__(self, blackboard):
        self.__blackboard = blackboard

    def executeFunction(self):
        # if round is final round then sabotage, else play things more tactically
        #print("as a spy i always sabotage")
        self.__blackboard.sabotage(True)
        return 1

    def __str__(self):
        return "SabotageSpy"


class SabotageResistance(CommandObject):
    def __init__(self,blackboard):
        self.__blackboard = blackboard

    def executeFunction(self):
        #print("as a resistance member i never sabotage")
        self.__blackboard.sabotage(False)
        return -1
    def __str__(self):
        return "SabotageResistance"

# if team has a spy on it then vote for it as spy
# if team doesn't have a spy on it then vote against it
class SpyVoting(CommandObject):
    def __init__(self, blackboard):
        self.__blackboard = blackboard

    def executeFunction(self):
        #print("getTeam as a spy")
        #print("team:" + str(self.__blackboard.getTeam()))
        for spy in self.__blackboard.getSpies():
            if spy in self.__blackboard.getTeam():
                self.__blackboard.setVoting(True)
                return 1
        self.__blackboard.setVoting(False)
        return 1

    def __str__(self):
        return "SpyVoting"

class ResistanceVoting(CommandObject):
    def __init__(self, blackboard):
        self.__blackboard = blackboard
    def executeFunction(self):
        # as a resistance fighter if spy is identified and is in team then presume spy else 1/team is spy
        identifiedSpies = self.__blackboard.getIdentifiedSpies()
        if len(identifiedSpies)!=0:
            for spy in identifiedSpies:
                if spy in self.__blackboard.getTeam():
                    #print("A spy " + str(spy) + " was identified and the resistance fighter voted against")
                    self.__blackboard.setVoting(False)
        else:
            self.__blackboard.setVoting(True)
        return 1

    def __str__(self):
        return "VotingResistance"
class ResistanceSelection(CommandObject):
    def __init__(self, blackboard):
        self.__blackboard = blackboard

    #TODO add memory for previous selections to not select a spy for the final mission
    def executeFunction(self):
        #print("select a team for a mission as a resistance fighter")
        # get identified spies and then omit an identified spy from the choice of team
        self.__blackboard.setSelectTeam([self.__blackboard.getBot()] + random.sample(self.__blackboard.getBot().others(), self.__blackboard.getCount() - 1))
        return 1
    def __str__(self):
        return "ResistanceSelection"
class SpySelection(CommandObject):
    def __init__(self,blackboard):
        self.__blackboard = blackboard

    def executeFunction(self):
        #print("select a team for a mission as a spy")
        self.__blackboard.setSelectTeam([self.__blackboard.getBot()] + random.sample(self.__blackboard.getBot().others(),self.__blackboard.getCount() - 1))
        return 1
    def __str__(self):
        return "SpySelection"