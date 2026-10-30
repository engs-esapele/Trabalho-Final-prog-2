#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <clocale>

using namespace std;
using namespace sf;

// ==================================================
// 1. ESTRUTURAS DO MAPA
// ==================================================

// Declara Sala antecipadamente, pois Saida utiliza Sala*
struct Sala;

// Representa uma conexao entre salas
struct Saida {
    Sala* destino;
    Saida* proxima;
};

// Representa uma sala do mapa
struct Sala {
    string nome;
    Saida* saidas;

    bool temChave;
    bool ehBoss;
};

// Lista encadeada que registra todas as salas
struct NoSala {
    Sala* sala;
    NoSala* proximo;
};

// Ponteiro para o inicio da lista de salas
NoSala* listaSalas = nullptr;


// ==================================================
// 2. CRIAR UMA SALA
// ==================================================

Sala* criarSala(string nome) {

    // Cria a sala dinamicamente
    Sala* novaSala = new Sala;

    novaSala->nome = nome;
    novaSala->saidas = nullptr;
    novaSala->temChave = false;
    novaSala->ehBoss = false;

    // Cria um no para registrar a sala
    NoSala* novoNo = new NoSala;

    novoNo->sala = novaSala;

    // Insere o no no inicio da lista
    novoNo->proximo = listaSalas;
    listaSalas = novoNo;

    return novaSala;
}


// ==================================================
// 3. CONECTAR DUAS SALAS
// ==================================================

void conectar(Sala* a, Sala* b) {

    // Cria uma saida de A para B
    Saida* saidaAB = new Saida;

    saidaAB->destino = b;
    saidaAB->proxima = a->saidas;
    a->saidas = saidaAB;

    // Cria uma saida de B para A
    Saida* saidaBA = new Saida;

    saidaBA->destino = a;
    saidaBA->proxima = b->saidas;
    b->saidas = saidaBA;
}


// ==================================================
// 4. MOSTRAR AS SAIDAS NO CONSOLE
// ==================================================

void mostrarSaidas(Sala* atual) {

    cout << "\nVoce esta em: " << atual->nome << endl;

    Saida* saida = atual->saidas;
    int numero = 1;

    // Percorre a lista de saidas
    while (saida != nullptr) {

        cout << numero << " - "
             << saida->destino->nome << endl;

        saida = saida->proxima;
        numero++;
    }

    cout << "0 - Sair da exploracao" << endl;
}


// ==================================================
// 5. MOVIMENTACAO ENTRE SALAS PELO CONSOLE
// ==================================================

bool moverJogador(Sala*& atual, int& chaves) {

    int escolha;

    cout << "Escolha um caminho: ";

    if (!(cin >> escolha)) {

        cin.clear();
        cin.ignore(10000, '\n');

        cout << "Entrada invalida!\n";
        return true;
    }

    // Encerra a exploracao
    if (escolha == 0) {
        return false;
    }

    // Procura a saida escolhida
    Saida* saida = atual->saidas;
    int numero = 1;

    while (saida != nullptr && numero < escolha) {
        saida = saida->proxima;
        numero++;
    }

    // Verifica se a saida existe
    if (escolha < 1 || saida == nullptr) {
        cout << "Opcao invalida!\n";
        return true;
    }

    // Descobre a sala escolhida
    Sala* destino = saida->destino;

    // Verifica se a sala e a do boss
    if (destino->ehBoss && chaves < 3) {

        cout << "\nPORTA BLOQUEADA!\n";
        cout << "Voce precisa de 3 chaves para entrar.\n";
        cout << "Chaves coletadas: " << chaves << "/3\n";

        return true;
    }

    // Atualiza a sala atual
    atual = destino;

    // Verifica se existe uma chave na sala
    if (atual->temChave) {

        chaves++;

        // Impede que a mesma chave seja coletada novamente
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


// ==================================================
// 6. LIBERAR A MEMORIA DO MAPA
// ==================================================

void liberarMapa() {

    NoSala* noAtual = listaSalas;

    // Percorre a lista de salas
    while (noAtual != nullptr) {

        NoSala* proximoNo = noAtual->proximo;
        Sala* salaAtual = noAtual->sala;

        // Libera as conexoes da sala
        Saida* saidaAtual = salaAtual->saidas;

        while (saidaAtual != nullptr) {

            Saida* proximaSaida = saidaAtual->proxima;

            delete saidaAtual;

            saidaAtual = proximaSaida;
        }

        // Libera a sala
        delete salaAtual;

        // Libera o no da lista
        delete noAtual;

        noAtual = proximoNo;
    }

    // Indica que a lista esta vazia
    listaSalas = nullptr;
}


// ==================================================
// 7. PROGRAMA PRINCIPAL
// ==================================================

int main() {

    setlocale(LC_ALL, "pt_BR.UTF-8");

    // ----------------------------------------------
    // CRIACAO DAS SALAS
    // ----------------------------------------------

    Sala* entrada = criarSala("Entrada");
    Sala* sala1 = criarSala("sala1");
    Sala* sala2 = criarSala("sala2");
    Sala* sala3 = criarSala("sala3");
    Sala* intermedio = criarSala("intermedio");
    Sala* boss = criarSala("sala do boss");

    // Define quais salas possuem chaves
    sala1->temChave = true;
    sala2->temChave = true;
    sala3->temChave = true;

    // Identifica a sala do boss
    boss->ehBoss = true;

    // ----------------------------------------------
    // CONEXOES ENTRE AS SALAS
    // ----------------------------------------------

    conectar(entrada, sala1);
    conectar(entrada, sala2);
    conectar(entrada, sala3);

    conectar(intermedio, sala1);
    conectar(intermedio, sala2);
    conectar(intermedio, sala3);

    conectar(intermedio, boss);

    // ----------------------------------------------
    // CARREGAMENTO DAS IMAGENS
    // ----------------------------------------------

    RenderWindow janela(
        VideoMode({512, 256}),
        "Meu RPG"
    );

    Texture texturaChao;

    if (!texturaChao.loadFromFile(
        "img/native/floors/floor_stone_1.png"
    )) {
        cout << "Erro ao carregar a imagem do chao!\n";
        liberarMapa();
        return 1;
    }

    texturaChao.setSmooth(false);

    Sprite chao(texturaChao);
    chao.setScale({2.0f, 2.0f});

    Texture texturaParede;

    if (!texturaParede.loadFromFile(
        "img/native/walls/wall_stone_face.png"
    )) {
        cout << "Erro ao carregar a imagem da parede!\n";
        liberarMapa();
        return 1;
    }

    texturaParede.setSmooth(false);

    Sprite parede(texturaParede);
    parede.setScale({2.0f, 2.0f});

    // ----------------------------------------------
    // LOOP PRINCIPAL DA JANELA
    // ----------------------------------------------

    while (janela.isOpen()) {

        // Verifica os eventos da janela
        while (const auto evento = janela.pollEvent()) {

            if (evento->is<Event::Closed>()) {
                janela.close();
            }
        }

        // Limpa a janela
        janela.clear(Color::Black);

        // ------------------------------------------
        // DESENHA O CHAO
        // ------------------------------------------

        for (int linha = 0; linha < 8; linha++) {

            for (int coluna = 0; coluna < 16; coluna++) {

                chao.setPosition({
                    static_cast<float>(coluna * 32),
                    static_cast<float>(linha * 32)
                });

                janela.draw(chao);
            }
        }

        // ------------------------------------------
        // PAREDE SUPERIOR
        // ------------------------------------------

        for (int coluna = 0; coluna < 16; coluna++) {

            parede.setPosition({
                static_cast<float>(coluna * 32),
                0.0f
            });

            janela.draw(parede);
        }

        // ------------------------------------------
        // PAREDE INFERIOR COM ABERTURA
        // ------------------------------------------

        for (int coluna = 0; coluna < 16; coluna++) {

            // Deixa uma abertura na coluna 7
            if (coluna == 7) {
                continue;
            }

            parede.setPosition({
                static_cast<float>(coluna * 32),
                224.0f
            });

            janela.draw(parede);
        }

        // ------------------------------------------
        // PAREDE ESQUERDA
        // ------------------------------------------

        for (int linha = 0; linha < 8; linha++) {

            parede.setPosition({
                0.0f,
                static_cast<float>(linha * 32)
            });

            janela.draw(parede);
        }

        // ------------------------------------------
        // PAREDE DIREITA
        // ------------------------------------------

        for (int linha = 0; linha < 8; linha++) {

            parede.setPosition({
                480.0f,
                static_cast<float>(linha * 32)
            });

            janela.draw(parede);
        }

        // Exibe tudo o que foi desenhado
        janela.display();
    }

    // Libera a memoria depois que a janela fecha
    liberarMapa();

    return 0;
}