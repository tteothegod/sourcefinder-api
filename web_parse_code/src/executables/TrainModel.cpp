#include <torch/torch.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>    
#include <algorithm>  


struct CitationClassifier : torch::nn::Module {
    torch::nn::Linear fc1{nullptr}, fc2{nullptr};
    
    CitationClassifier() {
        fc1 = register_module("fc1", torch::nn::Linear(500, 256));  // Input size 500 for max length
        fc2 = register_module("fc2", torch::nn::Linear(256, 1));
    }
    
    torch::Tensor forward(torch::Tensor x) {
        x = torch::relu(fc1(x));
        return torch::sigmoid(fc2(x));  
    }
};

torch::Tensor preprocess(const std::string& citation, int max_length) {
    std::vector<float> input_vector(max_length, 0.0f);
    int length = std::min(static_cast<int>(citation.size()), max_length);
    for (int i = 0; i < length; ++i) {
        input_vector[i] = static_cast<float>(citation[i]);  
    }
    return torch::tensor(input_vector, torch::dtype(torch::kFloat)).unsqueeze(0); 
}

int main() {
    std::vector<std::pair<std::string, int>> labeledCitations;

    std::ifstream file("labeled_citations.dat", std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    size_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));  

    // Load each pair (citation and label)
    for (size_t i = 0; i < size; ++i) {
        size_t citationLength;
        file.read(reinterpret_cast<char*>(&citationLength), sizeof(citationLength)); 
        std::string citation(citationLength, '\0');
        file.read(&citation[0], citationLength);  
        int label;
        file.read(reinterpret_cast<char*>(&label), sizeof(label));  
        labeledCitations.push_back({citation, label});
    }
    file.close();

    std::cout << "Data loaded from labeled_citations.dat" << std::endl;

    std::cout << "Data set size: " << labeledCitations.size() << std::endl;

    CitationClassifier model;
    
    // Optimizer & Loss function
    torch::optim::Adam optimizer(model.parameters(), torch::optim::AdamOptions(0.000001));  // Adjust learning rate
    auto criterion = torch::nn::BCELoss();
        

    // Training loop
    std::random_device rd;
    std::mt19937 g(rd());  // Random number generator

    for (int epoch = 0; epoch < 700; ++epoch) {
        float total_loss = 0;

        // Shuffle the dataset at the start of each epoch
        std::shuffle(labeledCitations.begin(), labeledCitations.end(), g);

        for (const auto& [citation, label] : labeledCitations) {
            torch::Tensor input = preprocess(citation, 500);  
            torch::Tensor target = torch::tensor({static_cast<float>(label)}).view({-1, 1}); 
            
            optimizer.zero_grad();
            torch::Tensor output = model.forward(input);
            torch::Tensor loss = criterion(output, target);
            loss.backward();
            optimizer.step();
            
            total_loss += loss.item<float>();
        }

        if (epoch % 10 == 0) std::cout << "Epoch " << epoch << " Loss: " << total_loss << std::endl;
    }

    // std::string testData = "^ostanza, Robert; d'Arge, Ralph; de Groot, Rudolf; Farber, Stephen; Grasso, Monica; Hannon, Bruce; Limburg, Karin; Naeem, Shahid; O'Neill, Robert V.; Paruelo, Jose; Raskin, Robert G.; Sutton, Paul; van den Belt, Marjan (May 1997).\"The value of the world's ecosystem services and natural capital\".Nature.387(6630):253–260.Bibcode:1997Natur.387..253C.doi:10.1038/387253a0."; // 1
    // std::string testData = "Raven, Peter H.;Johnson, George B.(2002).Biology(6th ed.). Boston, MA:McGraw-Hill. p. 68.ISBN0-07-112261-3.LCCN2001030052.OCLC45806501."; // 1
    std::string testData = "Mauseth JD (1991).Botany: an Introduction to Plant Biology. Philadelphia: Saunders College Publishing.ISBN9780030302220.p. 102: Cell division is cytokinesis, and nuclear division is karyokinesis. The words \"mitosis\" and \"meiosis\" technically refer only to karyokinesis but are frequently used to describe cytokinesis as well."; // 1
    // std::string testData = "Lambert, Jonathan (4 September 2020).\"Protecting half of Earth may help solve climate change, save species\".ScienceNews."; // 0
    torch::Tensor testInput = preprocess(testData, 500);  // Using 500 as the max length for input
    torch::Tensor estimation = model.forward(testInput);
    std::cout << "Expected 1 and got: " << estimation.item<float>() << std::endl;
    
    // Save model parameters
    torch::save(model.parameters(), "citation_model.pt");
    std::cout << "Model saved!" << std::endl;

    return 0;
}
