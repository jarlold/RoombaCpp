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
        NeuralNetworks::ActivationFunc f = &NeuralNetworks::tanhVec;
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
            solutions[i] = paul; //idk paul for today
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
            solutions[i].score = 0; // just in case
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
        if (solution.neuralNetwork.layers.size() == 0) {
            throw std::invalid_argument("A neural network without layers can't reproduce.");
        }
        
        // This is the problem isn't it
        Solution sol2;
        sol2 = solution;
        //sol2.neuralNetwork = NeuralNetworks::deepCopyNeuralNetwork(solution.neuralNetwork);
        NeuralNetworks::tweakNeuralNetwork(sol2.neuralNetwork, 0.1, 5); 
        return sol2;
    }
    
    // Make Shinzo Abe proud~!
    Solution sexualReproduction(const Solution& solution);
    
    // The actual genetic algorithm or something like that probably not important
    std::vector<Solution> fitSolutions(
        std::vector<Solution>& initialSolutions,
        std::function< int (Solution&)> scoringFunction,
        int iterations
    ) {
        //std::vector<Solution> initialSolutions = generateNSolutions(n, inputSize, numHiddenLayers, minHiddenSize, maxHiddenSize, outputSize);
        int n = initialSolutions.size();
        for (int i=0; i < iterations; i++) {
            testSolutions(initialSolutions, scoringFunction);
            doExtinction(initialSolutions, 0.2);
            int l = initialSolutions.size()-1;
            for (int i=l; i < n; i++) {
                int luckyGuy = rand() % (l);
                initialSolutions.push_back(
                    asexualReproduction(initialSolutions[i])
                );
            }
        }
        
        return initialSolutions;
    }
}
