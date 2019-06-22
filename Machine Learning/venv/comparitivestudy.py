from sklearn.model_selection import KFold, cross_val_score
from sklearn.tree import DecisionTreeClassifier
from sklearn import tree
from sklearn import svm
from sklearn import neighbors
from sklearn.model_selection import KFold, cross_val_score
from sklearn.metrics import cohen_kappa_score, confusion_matrix
from sklearn.model_selection import StratifiedKFold
import pandas as pd
import graphviz
import matplotlib.pyplot as plt

class DecisionTree:
    # pruned tree classifyer
    def __init__(self, train_data,train_class_data):
        k_fold = KFold(n_splits=10)
        self.dt = DecisionTreeClassifier(max_leaf_nodes=80, criterion="entropy")
        cross_val_score_tree = cross_val_score(self.dt, train_data, train_class_data, cv=k_fold, n_jobs=-1)
        self.dt.fit(train_data,train_class_data)
        #print(cross_val_score_tree)

    def visualiseTree(self):
        exported_tree = tree.export_graphviz(self.dt,out_file=None)
        exported_graph = graphviz.Source(exported_tree)
        exported_graph.render("Restaurants")
    def predict(self,test_data):
        return self.dt.predict(test_data)

    def scoreTree(self, training_data, training_class_data):
        k_fold = KFold(n_splits=10)
        cm = confusion_matrix(training_class_data,self.dt.predict(training_data))
        kappa = cohen_kappa_score(training_class_data,self.dt.predict(training_data))
        print("Statistics for decision tree implementation: ")
        print(cross_val_score_tree)
        print(kappa)
        plt.matshow(cm)
        plt.title('DecisionTree confusion matrix')
        plt.colorbar()
        plt.ylabel('True label')
        plt.xlabel('Predicted label')
        plt.show()

class SVM:
    def __init__(self,train_data,train_class_data):

       self._svm = svm.SVC(gamma='scale',decision_function_shape='ovo')
       self.dt.fit(train_data, train_class_data)
       cvs = cross_val_score(self._svm, training_data, training_class_data, cv=k_fold, n_jobs=-1)
       print("SVM cvs: ")
       print(cvs)

    def scoreSVM(self,training_data,training_class_data):
        print("SVM statistics:")
        k_fold = KFold(n_splits=10)
        cm = confusion_matrix(training_class_data,self._svm.predict(training_data))

        cks = cohen_kappa_score(training_class_data,self._svm.predict(training_data))
        print("cvs:")
        print(cvs)
        print("cks: ")
        print(cks)

        plt.matshow(cm)
        plt.title('SVM confusion matrix')
        plt.colorbar()
        plt.ylabel('True label')
        plt.xlabel('Predicted label')
        plt.show()

## https://kevinzakka.github.io/2016/07/13/k-nearest-neighbor/
class KNN:
    def __init__(self,train_data,train_class_data):
        self.__knn = neighbors.KNeighborsClassifier(n_neighbors=14)
        k_fold = KFold(n_splits=10)
        self.dt.fit(train_data, train_class_data)
        cvs = cross_val_score(self._svm, training_data, training_class_data, cv=k_fold, n_jobs=-1)
        print("SVM cvs: ")
        print(cvs)

    def scoreNN(self,training_data,training_class_data):
        cks = cohen_kappa_score(training_class_data,self.__knn.predict(training_data))
        cm = confusion_matrix(training_class_data,self.__knn.predict(training_data))
        print('cvs:')
        print(cvs)
        print("cks:")
        print(cks)
        plt.matshow(cm)
        plt.title('KNN confusion matrix')
        plt.colorbar()
        plt.ylabel('True label')
        plt.xlabel('Predicted label')
        plt.show()

def seperateData(training_file):
    regular_training_data = training_file.iloc[:,:-1]
    training_class_data = training_file.iloc[:,-1:]
    return regular_training_data,training_class_data

def saveToTestDataClassifications(test_data,predictions):
    for lineOfTestData in test_data:
        for lineOfPredictions in predictions:
            print(str(lineOfTestData) + ',' + str(lineOfPredictions))

def main():

    test_filename = "CE802_Ass_2018_Test.csv"
    training_filename = "CE802_Ass_2018_Data.csv"
    train_df = pd.read_csv(training_filename)
    test_df = pd.read_csv(test_filename)
    training_data,training_shop_class = seperateData(train_df)
    test_data, test_shop_class = seperateData(test_df)
    udt = DecisionTree(training_data,training_shop_class)
    predictions  = udt.predict(test_data)
    print("test data")
    print(test_data)
    saveToTestDataClassifications(test_data,predictions)
    #support_vector_machine = SVM(training_data,training_shop_class)
    #support_vector_machine.scoreSVM(training_data,training_shop_class)

    #knn = KNN(training_data,training_shop_class)
    #knn.scoreNN(training_data,training_shop_class)

if __name__ == '__main__':
    main()