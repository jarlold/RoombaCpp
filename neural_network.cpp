#pragma once
#include <stdexcept>
#include <vector>
#include <functional>
#include <random>

namespace NeuralNetworks {

using ActivationFunc = std::function<std::vector<float>(std::vector<float>&)>;

struct Layer {
    std::vector<float> weights;
    int weightsWidth;
    ActivationFunc activationFunction;
};

struct NeuralNetwork {
    std::vector<Layer> layers;
};

// Activation functions
std::vector<float> sigmoidVec(std::vector<float>& input) {
    int m = input.size();
    for (int i=0; i < m; i++) {
        // Fast sigmoid as recommended by a random stack overflow page
        input[i] = input[i] / (1 + abs(input[i]));
    }
    return input;
}

std::vector<float> noActivationVec(std::vector<float>& input) {
    return input;
}

NeuralNetwork buildNeuralNetwork(std::vector<int>& layerSizes, ActivationFunc& f) {
    int numLayers = layerSizes.size();
    std::vector<Layer> layers(numLayers-1);
    
    for (int i=0; i < numLayers-1; i++) {
        std::vector<float> weights(layerSizes[i]*layerSizes[i+1], 1.0);
        int w = layerSizes[i+1];
        layers[i] = (Layer) {weights, w, f};
    }
    
    return (NeuralNetwork) { layers };   
}

void tweakNeuralNetwork(NeuralNetwork& nn, float mutationStrength, int mutationRate) {
    float r;
    for (int i =0; i < mutationRate; i++) {
        r = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(mutationStrength*2))) - mutationStrength;
        std::uniform_real_distribution<float> distr(-mutationStrength, mutationStrength);
        int pos1 = rand() % nn.layers.size();
        int pos2 = rand() % nn.layers[pos1].weights.size();
        nn.layers[pos1].weights[pos2] += r;
    }
}

// Basic matrix operations that probably exist already in a library
std::vector<float> vectorMatrixMultiplication(const std::vector<float>& vec, const std::vector<float>& matrix, int matrixWidth) {
    /*
              w1 w2  w3  w4                    w1 *i1 + w5 *i2 + w9  *i3
              w5 w6  w7  w8   *  i1 i2 i3   =  w2 *i1 + w6 *i2 + w10 *i3
              w9 w10 w11 w12                   w3 *i1 + w7 *i2 + w11 *i3
                                               w4 *i1 + w8 *i2 + w12 *i3
    */
    
    int vLength = vec.size();
    int matrixLength = matrix.size() / matrixWidth;
    
    if (vLength != matrixLength) {
        printf("ERROR %d %d \r\n", vLength, matrixLength);
        throw std::invalid_argument("Bad dimensions in vector matrix multiplication.");
    }
    
    std::vector<float> outputs(matrixWidth, 0.0f);
    
    for (int i =0 ; i<matrixWidth; i++) {
        for (int j =0; j < matrixLength; j++) {
            outputs[i] += matrix[j*matrixWidth + i] * vec[j]; 
        }
    }

    return outputs;
}

/*
float tanh(std::vector<float>& input);
float relu(std::vector<float>& input);
float leakyRelu((std::vector<float>& input);
*/

std::vector<float> feedForwardLayer(Layer& layer, std::vector<float>& input) {
    std::vector<float> m = vectorMatrixMultiplication(input, layer.weights, layer.weightsWidth);
    return layer.activationFunction(m);
}

std::vector<float> doPrediction(NeuralNetwork& network, std::vector<float>& input) {
    int b = network.layers.size();
    std::vector<float> lastOut = input;
    for (int i=0; i<b; i++) {
        lastOut = feedForwardLayer(network.layers[i], lastOut);
    }
    return lastOut;
}

void neuralNetworkTestCase() {
    // Test matrix math
    printf("Running test case for matrix math...\r\n");
    std::vector<float> v = { 13, 14, 15 };
    std::vector<float> m = {
        1.0, 2.0,  3.0,  4.0,
        5.0, 6.0,  7.0,  8.0,
        9.0, 10.0, 11.0, 12.0
    };
    
    std::array<float, 4> answer = {218, 260, 302, 244};
    std::vector<float> tout = vectorMatrixMultiplication(v, m, 4);
    for (int i=0; i<3; i++) {
        if (tout[i] != answer[i]) {
            throw std::runtime_error("Matrix vector multiplication is not working right.");
        }
    }
    
    // Test the layer and feed forward
    ActivationFunc f = &noActivationVec;
    std::vector<float> w1(4*5, 1.0);
    std::vector<float> w2(5*6, 1.0);
    Layer l1 = (Layer) { w1, 5, f};
    Layer l2 = (Layer) { w2, 6, f};
    
    // Arguably a bad decision to have all the inputs be 1.0 here, but if vector-matrix mult
    // was bad it would show in the above and i don't want to change it.
    std::vector<float> in1 = {1.0, 1.0, 1.0, 1.0};
    std::vector<float> out1 = feedForwardLayer( l1, in1);
    std::vector<float> out2 = feedForwardLayer(l2, out1);

    // Test the network builder
    printf("Network Test:\r\n");
    std::vector<int> layerSizes = {4, 5, 6};
    NeuralNetwork nn = buildNeuralNetwork(layerSizes, f);
    
    std::vector<float> testInput = {1, 1, 1, 1};
    tout = doPrediction(nn, testInput);
    
    for (int i = 0; i < 4; i++) {
        if (out2[i] != tout[2] || tout[2] != 20.0) {
            throw std::runtime_error("Neural network forward pass is broken.");
        }
    }
    
}

}





