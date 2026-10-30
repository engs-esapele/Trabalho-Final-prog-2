#include <SFML/Graphics.hpp> //janelas, imagens, texturas e sprites.
#include <iostream>
#include <string>
#include <clocale> //axibição de acentos 

using namespace std;

// Declara a estrutura Sala antecipadamente
struct Sala;

// Representa uma conexão entre salas
struct Saida {
    Sala* destino;     // Sala para onde a conexão leva
    Saida* proxima;    // Próxima conexão da lista
};

// Representa uma sala do mapa
struct Sala {
    string nome;
    Saida* saidas;

    bool temChave;  // Indica se ainda existe uma chave na sala
    bool ehBoss;    // Indica se esta é a sala do boss
};

// Lista encadeada que registra todas as salas criadas
struct NoSala {
    Sala* sala;
    NoSala* proximo;
};

// Ponteiro para o início da lista de salas
NoSala* listaSalas = nullptr;


// Cria uma sala e registra sua existência
Sala* criarSala(string nome) {
    // Cria a sala dinamicamente
    Sala* novaSala = new Sala;

    novaSala->nome = nome;
    novaSala->saidas = nullptr;

    novaSala->temChave = false;
    novaSala->ehBoss = false;

    // Cria um nó para registrar a sala
    NoSala* novoNo = new NoSala;

    novoNo->sala = novaSala;

    // Insere o nó no início da lista de salas
    novoNo->proximo = listaSalas;
    listaSalas = novoNo;

    return novaSala;
}


// Conecta duas salas nos dois sentidos
void conectar(Sala* a, Sala* b) {
    // Cria uma saída de A para B
    Saida* saidaAB = new Saida;

    saidaAB->destino = b;
    saidaAB->proxima = a->saidas;
    a->saidas = saidaAB;

    // Cria uma saída de B para A
    Saida* saidaBA = new Saida;

    saidaBA->destino = a;
    saidaBA->proxima = b->saidas;
    b->saidas = saidaBA;
}


// Mostra as saídas disponíveis na sala atual
void mostrarSaidas(Sala* atual) {
    cout << "\nVoce esta em: " << atual->nome << endl;

    Saida* saida = atual->saidas;
    int numero = 1;

    // Percorre a lista de saídas
    while (saida != nullptr) {
        cout << numero << " - "
             << saida->destino->nome << endl;

        saida = saida->proxima;
        numero++;
    }

    cout << "0 - Sair da exploracao" << endl;
}


// Permite escolher uma saída
bool moverJogador(Sala*& atual, int& chaves) {
    int escolha;

    cout << "Escolha um caminho: ";

    if (!(cin >> escolha)) {
        // Encerra se a entrada não for um número
        cin.clear();
        cin.ignore(10000, '\n');

        cout << "Entrada invalida!\n";
        return true;
    }

    // Encerra a exploração
    if (escolha == 0) {
        return false;
    }

    // Procura a saída escolhida
    Saida* saida = atual->saidas;
    int numero = 1;

    while (saida != nullptr && numero < escolha) {
        saida = saida->proxima;
        numero++;
    }

    // Verifica se a saída existe
    if (escolha < 1 || saida == nullptr) {
        cout << "Opcao invalida!\n";
        return true;
    }

   // Descobre qual sala o jogador escolheu
Sala* destino = saida->destino;

// Verifica se a sala escolhida é a do boss
if (destino->ehBoss && chaves < 3) {
    cout << "\nPORTA BLOQUEADA!\n";
    cout << "Voce precisa de 3 chaves para entrar.\n";
    cout << "Chaves coletadas: " << chaves << "/3\n";

    return true;
}

// O jogador pode entrar na sala
atual = destino;

// Verifica se existe uma chave na sala
if (atual->temChave) {
    chaves++;

    // A chave não pode ser coletada novamente
    atual->temChave = false;

    cout << "\nVoce encontrou uma chave!\n";
    cout << "Chaves coletadas: " << chaves << "/3\n";
}

// Avisa quando o jogador entra na sala do boss
if (atual->ehBoss) {
    cout << "\nVoce entrou na sala do BOSS!\n";
}

return true;
}


// Libera toda a memória utilizada pelo mapa
void liberarMapa() {
    NoSala* noAtual = listaSalas;

    // Percorre a lista que registra todas as salas
    while (noAtual != nullptr) {
        NoSala* proximoNo = noAtual->proximo;

        Sala* salaAtual = noAtual->sala;

        // Libera todas as conexões desta sala
        Saida* saidaAtual = salaAtual->saidas;

        while (saidaAtual != nullptr) {
            Saida* proximaSaida = saidaAtual->proxima;

            delete saidaAtual;

            saidaAtual = proximaSaida;
        }

        // Libera a sala
        delete salaAtual;

        // Libera o nó que registrava a sala
        delete noAtual;

        noAtual = proximoNo;
    }

    // A lista agora está vazia
    listaSalas = nullptr;
}


int main() {
    setlocale(LC_ALL, "pt_BR.UTF-8");

    int chaves = 0;

    // Cria as salas e registra cada ponteiro no vetor
    Sala* entrada = criarSala("Entrada");  
    Sala* sala1 = criarSala("sala1");
    Sala* sala2 = criarSala("sala2");
    Sala* sala3 = criarSala("sala3");
    Sala* intermedio = criarSala("intermedio");
    Sala* boss = criarSala("sala do boss");

    // chaves do mapa
sala1->temChave = true;
sala2->temChave = true;
sala3->temChave = true;

// Identifica a sala do boss
boss->ehBoss = true;

    // Conecta as salas
    conectar(entrada, sala1);
    conectar(entrada, sala2);
    conectar(entrada, sala3);
    conectar(intermedio, sala1);
    conectar(intermedio, sala2);
    conectar(intermedio, sala3);
    conectar(intermedio, boss);


    // Define a posição inicial do jogador
    Sala* atual = entrada;

    bool explorando = true;
  // Continua até o jogador decidir sair
while (explorando) {
    mostrarSaidas(atual);

    cout << "Chaves: " << chaves << "/3\n";

    explorando = moverJogador(atual, chaves);
}

    cout << "\nExploracao encerrada!\n";

    // Libera todas as conexões, salas e nós
    liberarMapa();


}
