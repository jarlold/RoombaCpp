#pragma once
#include <vector>
#include <functional>
#include "neural_network.cpp"

namespace NeuroEvolution {
    struct Solution {
        NeuralNetworks::NeuralNetwork nn;
    };
    
    Solution generateSolution(int inputSize, int numHiddenLayers, int minHiddenSize, int maxHiddenSize, int outputSize ) {
        // Come up with some amount of layers of some amount of size or something like that
        int numLayers = 2 + (rand() % numLayers);
        std::vector<int> layerSizes(numLayers);
        for (int i=0; i < numLayers; i++) {
            layerSizes.push_back( minHiddenSize + (rand() % maxHiddenSize) );
        }
        
        // Then do the um the like the the network and like yeah bro
        NeuralNetworks::ActivationFunc f = &NeuralNetworks::sigmoidVec;
        NeuralNetworks::NeuralNetwork nn = NeuralNetworks::buildNeuralNetwork(
            layerSizes,
            f
        );
        
        // They call it casting because when you go fly-fishing you make a lot of swoopy curve movements
        // that look like the bracers.
        Solution s = (Solution) { nn };
        return s;
    }
    
    std::vector<Solution> generateNSolutions(int n, int inputSize, int numHiddenLayers, int minHiddenSize, int maxHiddenSize, int outputSize) {
        std::vector<Solution> solutions(n);
        for (int i=0; i<n; i++) {
            Solution paul = generateSolution(inputSize, numHiddenlayers, minHiddenSize, maxHiddenSize, outputSize);
            solutions.push_back(paul);
        }
        return solutions;
    }


}
