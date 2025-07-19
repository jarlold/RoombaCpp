#pragma once
#include <vector>
#include <functional>
#include "neural_network.cpp"

namespace NeuroEvolution {
    struct Solution {
        NeuralNetworks::NeuralNetwork neuralNetwork;
        int score;
    };
    
    Solution generateSolution(int inputSize, int numHiddenLayers, int minHiddenSize, int maxHiddenSize, int outputSize ) {
        // Come up with some amount of layers of some amount of size or something like that
        int numLayers = rand();
        numLayers = numLayers % numHiddenLayers;
        numLayers += 2;
        std::vector<int> layerSizes(numLayers);
        layerSizes[0] = inputSize;
        layerSizes[numLayers-1] = outputSize;
        
        for (int i=1; i < numLayers-1; i++) {
            int lsize = rand();
            lsize = lsize % (maxHiddenSize - minHiddenSize);
            lsize += minHiddenSize;
            layerSizes[i] = lsize;
        }
        
        // Then do the um the like the the network and like yeah bro
        NeuralNetworks::ActivationFunc f = &NeuralNetworks::sigmoidVec;
        NeuralNetworks::NeuralNetwork nn = NeuralNetworks::buildNeuralNetwork(
            layerSizes,
            f
        );
        
        // They call it casting because when you go fly-fishing you make a lot of swoopy curve movements
        // that look like the bracers.
        Solution s = (Solution) { nn, 0 };
        return s;
    }
    
    // Generate a bunch of solutions
    std::vector<Solution> generateNSolutions(int n, int inputSize, int numHiddenLayers, int minHiddenSize, int maxHiddenSize, int outputSize) {
        std::vector<Solution> solutions(n);
        for (int i=0; i<n; i++) {
            Solution paul = generateSolution(inputSize, numHiddenLayers, minHiddenSize, maxHiddenSize, outputSize);
            solutions.push_back(paul);
        }
        return solutions;
    }
    
    bool compareSolutionScores(const Solution& a, const Solution& b) {
        return a.score < b.score;
    }
    
    // Take a scoring function and apply it to all the possible solutions. Then sort the list of solutions
    // from best to worst.
    void testSolutions(std::vector<Solution>& solutions, std::function< int (Solution&)> scoringFunction) {
        // Roombas must face a trial to test the worth of their so(u)ls.
        int sols = solutions.size();
        for (int i =0; i < sols; i++) {
            solutions[i].score = scoringFunction(solutions[i]);
        }
        
        // Roombas who are pure of heart and strong of arm will reach the top of the world
        // whereas the corrupt sinner roombas will be left at the bottom of the vector, where
        // they will be cast from the world of living, and enter the lake of fire.
        std::sort(solutions.begin(), solutions.end(), compareSolutionScores);
    }
    
    // KILL JOHN LENNON. KILL JOHN LENNON!
    void doExtinction(std::vector<Solution>& solutions, float murderRate) {
        int sizeOfSols = solutions.size();
        int toKill = floor(sizeOfSols*murderRate);
        for (int i = sizeOfSols-toKill; i < sizeOfSols; i++) {
            solutions.pop_back();
        }
    }
    
    // Sexy time, get busy (doing sex) (without a partner)
    Solution asexualReproduction(const Solution& solution) {
        Solution sol2;
        sol2 = solution;
        int f = sol2.neuralNetwork.layers[0].weights.size();
        for (int i =0; i < f; i++) {
            sol2.neuralNetwork.layers[0].weights[i] = 0;
        }   
        return sol2;
    }
    
    // Make Shinzo Abe proud~!
    Solution sexualReproduction(const Solution& solution);

}
