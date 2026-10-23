
#include <iostream>
#include <string>
#include <clocale>
#include <vector>

using namespace std;

// Representa uma sala do mapa
struct Sala {
    string nome;
    vector<Sala*> saidas; // Ponteiros para as salas conectadas
};

// Cria uma sala na memória dinâmica
Sala* criarSala(string nome) {
    Sala* novaSala = new Sala;

    novaSala->nome = nome;

    return novaSala;
}

// Conecta duas salas nos dois sentidos
void conectar(Sala* a, Sala* b) {
    a->saidas.push_back(b);
    b->saidas.push_back(a);
}

// Mostra a sala atual e suas saídas
void mostrarSaidas(Sala* atual) {
    cout << "\nVoce esta em: " << atual->nome << endl;

    for (int i = 0;
         i < static_cast<int>(atual->saidas.size());
         i++) {
        cout << i + 1 << " - "
             << atual->saidas[i]->nome << endl;
    }

    cout << "0 - Sair da exploração" << endl;
}

// Permite escolher uma saída ou encerrar a exploração
bool moverJogador(Sala*& atual) {
    int escolha;

    cout << "Escolha um caminho: ";
    cin >> escolha;

    // Encerra a exploração
    if (escolha == 0) {
        return false;
    }

    // Verifica se a escolha existe
    if (escolha < 1 ||
        escolha > static_cast<int>(atual->saidas.size())) {
        cout << "Opção invalida!\n";
        return true;
    }

    // Atualiza o ponteiro para a sala escolhida
    atual = atual->saidas[escolha - 1];

    return true;
}

int main() {
    setlocale(LC_ALL, "pt_BR.UTF-8");
    // Guarda os ponteiros de todas as salas criadas
    vector<Sala*> salas;

    // Cria as salas e registra cada ponteiro no vetor
    Sala* entrada = criarSala("Entrada");
    salas.push_back(entrada);

    Sala* corredor = criarSala("Corredor");
    salas.push_back(corredor);

    Sala* tesouro = criarSala("Sala do Tesouro");
    salas.push_back(tesouro);

    Sala* armadilha = criarSala("Sala da Armadilha");
    salas.push_back(armadilha);

    // Conecta as salas
    conectar(entrada, corredor);
    conectar(entrada, tesouro);
    conectar(entrada, armadilha);
    conectar(corredor, tesouro);

    // Define a posição inicial do jogador
    Sala* atual = entrada;

    bool explorando = true;

    // Repete até o jogador escolher sair
    while (explorando) {
        mostrarSaidas(atual);
        explorando = moverJogador(atual);
    }

    cout << "Exploração encerrada!\n";

    // Libera a memória de cada sala uma única vez
    for (Sala* sala : salas) {
        delete sala;
    }

    // Remove os ponteiros do vetor
    salas.clear();


}
