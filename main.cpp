#include <SFML/Graphics.hpp>    
#include <iostream>
#include <string>
#include <clocale>
#include <random> // numeros aleatorios para movimentacao
#include <cmath> //distancia e movimento
#include <algorithm> // min, max
#include <cctype>
#include <optional>
using namespace std;
using namespace sf;

struct Item {
    string nome;
    int atributo; 
    Item* proximo;
};

struct Monstro {
    string nome;
    int vida;
    int ataque;
};
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
    Item* itens = nullptr;      // Inicializa a lista de itens como vazianicializa a lista de itens como vazia
    Monstro* monstro = nullptr; // Inicializa sem monstro

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
///adcionando itens e monstros nas salas
void adicionarItem(Sala* sala, string nome, int atributo) {
    Item* novoItem = new Item;
    novoItem->nome = nome;
    novoItem->atributo = atributo;
    novoItem->proximo = sala->itens;
    sala->itens = novoItem;
}

void adicionarMonstro(Sala* sala, string nome, int vida, int ataque) {
    Monstro* novoMonstro = new Monstro;
    novoMonstro->nome = nome;
    novoMonstro->vida = vida;
    novoMonstro->ataque = ataque;
    sala->monstro = novoMonstro;
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
        if (salaAtual->monstro != nullptr) {
            delete salaAtual->monstro;
        }
        Item* itemAtual = salaAtual->itens;
        while (itemAtual != nullptr) {
            Item* prox = itemAtual->proximo;
            delete itemAtual;
            itemAtual = prox;
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
// ESTRUTURS DO JOGADOR E INVENTARIO - EMILY
// ==================================================
//informa qual efeito o item possui
enum class EfeitoItemRPG{
    COMUM,
    POCAO,
    ARMADURA
};
//cada ekemento guarda um item no inventario
struct NoInventarioRPG{
    string nome;
    int atributo;
    int quantidade;
    NoInventarioRPG* proximo;
};
// A pilha guarda as pocoes: a ultima coletada e a primeira usada.
struct NoPocaoRPG{
    string nome;
    int cura;
    NoPocaoRPG* proximo;
};

//informacoes do jogador
struct JogadorRPG{
    RectangleShape corpo;
    optional<Sprite> sprite;
    int vida = 100;
    int vidaMax = 100;
    int resistenciaArmadura = 0;
    int resistenciaMax =100;
    bool armaduraColetada = false;

    float velocidade = 150.0f;
    float tempoVulneravel = 0.0f;
    float tempoAtaque = 0.0f;

// Dados do inventário
    NoInventarioRPG* inventario = nullptr;
    NoPocaoRPG* topoPocoes = nullptr;
    int quantidadePocoes = 0;
    int itensRecusados=0;

// Controle das teclas
    bool inventarioAberto = false;
    bool teclaIAnterior = false;
    bool teclaEAnterior = false;
    bool teclaQAnterior = false;
    bool teclaHAnterior = false;
    bool teclaEspacoAnterior = false;

    int chavesRegistradas = 0;

    JogadorRPG() {
        corpo.setSize({18.0f, 24.0f});
        corpo.setFillColor(Color::Blue);
        corpo.setPosition({245.0f, 120.0f});
    }
};

// ============================================================
// 2. FUNCOES DO INVENTARIO
// ============================================================

// Procura um item pelo nome usando recursividade.
NoInventarioRPG* buscarItemRecursivoRPG(
    NoInventarioRPG* atual,
    const string& nome
) {
    if (atual == nullptr) {
        return nullptr;
    }

    if (atual->nome == nome) {
        return atual;
    }

    return buscarItemRecursivoRPG(atual->proximo, nome);
}

// Adiciona um item ao inventario.
// Se ele ja existir, aumenta a quantidade.
void adicionarAoInventarioRPG(
    JogadorRPG& jogador,
    const string& nome,
    int atributo
) {
    NoInventarioRPG* itemEncontrado =
        buscarItemRecursivoRPG(jogador.inventario, nome);

    if (itemEncontrado != nullptr) {
        itemEncontrado->quantidade++;
        return;
    }

    NoInventarioRPG* novoItem = new NoInventarioRPG;

    novoItem->nome = nome;
    novoItem->atributo = atributo;
    novoItem->quantidade = 1;
    novoItem->proximo = jogador.inventario;

    jogador.inventario = novoItem;
}

// Remove uma unidade de um item do inventário.
// Parâmetros: jogador e nome do item.
// Retorno: não possui.
void removerUmItemInventarioRPG(
    JogadorRPG& jogador,
    const string& nome
) {
    NoInventarioRPG* atual = jogador.inventario;
    NoInventarioRPG* anterior = nullptr;

    while (atual != nullptr && atual->nome != nome) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual == nullptr) return;
    if (atual->quantidade > 1) {
        atual->quantidade--;
        return;
    }

    if (anterior == nullptr) jogador.inventario = atual->proximo;
    else anterior->proximo = atual->proximo;

    delete atual;
}

// Organiza os itens em ordem alfabetica usando Selection Sort.
// Analise de complexidade (n = quantidade de itens distintos no inventario):
//  - Comparacoes: sempre n(n-1)/2, ou seja O(n^2) no melhor, medio e pior caso.
//  - Trocas: no maximo n-1, ou seja O(n).
//  - Espaco extra: O(1), pois ordena no proprio inventario trocando o
//    conteudo dos nos.
//  - Nao e estavel: a troca pode inverter a ordem de elementos iguais.
void ordenarInventarioRPG(NoInventarioRPG* inicio) {
    NoInventarioRPG* atual = inicio;

while (atual != nullptr) {
    NoInventarioRPG* menor = atual;
    NoInventarioRPG* proximo = atual->proximo;

while (proximo != nullptr) {
    if (proximo->nome < menor->nome) {
    menor = proximo;
}

proximo = proximo->proximo;
        }

if (menor != atual) {
     swap(atual->nome, menor->nome);
     swap(atual->atributo, menor->atributo);
    swap(atual->quantidade, menor->quantidade);
}
 atual = atual->proximo;
}
}

// Mostra os itens usando recursividade.
void mostrarItensRecursivoRPG(NoInventarioRPG* atual) {
    if (atual == nullptr) {
        return;
    }

cout << atual->nome
 << " | Quantidade: " << atual->quantidade
 << " | Atributo: " << atual->atributo << '\n';
    mostrarItensRecursivoRPG(atual->proximo);
}

// Mostra o inventario completo.
void mostrarInventarioRPG(JogadorRPG& jogador) {
    ordenarInventarioRPG(jogador.inventario);

cout << "\n========== INVENTARIO ==========\n";

    if (jogador.inventario == nullptr) {
    cout << "Inventario vazio.\n";
    } 
    else {
        mostrarItensRecursivoRPG(jogador.inventario);
    }

    cout << "Pocoes: " << jogador.quantidadePocoes << '\n';
    cout << "Chaves: " << jogador.chavesRegistradas << "/3\n";
    cout << "Vida: " << jogador.vida << "/" << jogador.vidaMax << '\n';
    cout << "Armadura: " << jogador.resistenciaArmadura << "/" << jogador.resistenciaMax << '\n';
}

// ============================================================
// 3. PILHA DE POCOES
// ============================================================

// Coloca uma pocao no topo da pilha.
void empilharPocaoRPG(
    JogadorRPG& jogador,
    const string& nome,
    int cura
) {
    NoPocaoRPG* novaPocao = new NoPocaoRPG;
    novaPocao->nome = nome;
    novaPocao->cura = cura;
    novaPocao->proximo = jogador.topoPocoes;
    jogador.topoPocoes = novaPocao;
    jogador.quantidadePocoes++;
}

// Usa a ultima pocao que foi coletada.
void usarPocaoRPG(JogadorRPG& jogador) {
    if (jogador.topoPocoes == nullptr) {
    cout << "\nVoce nao possui pocoes.\n";
        return;
    }

    if (jogador.vida == jogador.vidaMax) {
    cout << "\nSua vida ja esta completa.\n";
        return;
    }

    NoPocaoRPG* pocaoUsada = jogador.topoPocoes;
    jogador.topoPocoes = pocaoUsada->proximo;
    jogador.vida = jogador.vida + pocaoUsada->cura;

    if (jogador.vida > jogador.vidaMax) {
    jogador.vida = jogador.vidaMax;
    }

    removerUmItemInventarioRPG(jogador, pocaoUsada->nome);
    jogador.quantidadePocoes--;

    cout << "\nPocao utilizada. Vida: " << jogador.vida << "/" << jogador.vidaMax << '\n';
    delete pocaoUsada;
}

// ============================================================
// 4. ITENS NO CHAO: POSICAO, COLETA E RECUSA
// ============================================================

// Como a estrutura Item do grupo nao possui posicao,
// esta estrutura guarda a posicao visual de cada item.
struct EstadoItemRPG {
    Item* referencia;
    sf::CircleShape desenho;
    optional<sf::Sprite> sprite; //para colocar a foto do item
    EfeitoItemRPG efeito;
    bool avisoMostrado;
    EstadoItemRPG* proximo;
};

EstadoItemRPG* estadosItensRPG = nullptr;

// Descobre o tipo do item pelo nome.
EfeitoItemRPG identificarEfeitoItemRPG(const std::string& nome) {
    if (nome.find("Pocao") != string::npos || nome.find("pocao") != string::npos ||
 nome.find("Poção") != string::npos || nome.find("poção") != string::npos) {
    return EfeitoItemRPG::POCAO;
    }

    if (nome.find("Armadura") != string::npos ||  nome.find("armadura") != string::npos) {
        return EfeitoItemRPG::ARMADURA;
    }
    return EfeitoItemRPG::COMUM;
}
// Aplica uma textura de pixel art a um item no chao.
void configurarSpriteItemRPG(EstadoItemRPG* estado, const sf::Texture& textura) {
    if (estado == nullptr) return;
    estado->sprite.emplace(textura);
    sf::Vector2u tam = textura.getSize();
    estado->sprite->setScale({
        18.0f / static_cast<float>(tam.x),
        18.0f / static_cast<float>(tam.y)
    });
}

// Aplica texturas de pixel art a todos os itens criados no mapa.
void aplicarTexturasItensRPG(
    const sf::Texture& texPocao,
    const sf::Texture& texArmadura,
    const sf::Texture& texComum
) {
    EstadoItemRPG* atual = estadosItensRPG;
    while (atual != nullptr) {
        if (atual->efeito == EfeitoItemRPG::POCAO && texPocao.getSize().x > 0) {
            configurarSpriteItemRPG(atual, texPocao);
        } else if (atual->efeito == EfeitoItemRPG::ARMADURA && texArmadura.getSize().x > 0) {
            configurarSpriteItemRPG(atual, texArmadura);
        } else if (texComum.getSize().x > 0) {
            configurarSpriteItemRPG(atual, texComum);
        }
        atual = atual->proximo;
    }
}

// Procura os dados visuais do item.
// Se ainda nao existirem, cria esses dados.
EstadoItemRPG* obterEstadoItemRPG(
    Item* item,
    sf::Vector2f posicao,
    EfeitoItemRPG efeito = EfeitoItemRPG::COMUM
) {
    EstadoItemRPG* atual = estadosItensRPG;

    while (atual != nullptr) {
        if (atual->referencia == item) {
            return atual;
        }

        atual = atual->proximo;
    }

    if (efeito == EfeitoItemRPG::COMUM) {
        efeito = identificarEfeitoItemRPG(item->nome);
    }

    EstadoItemRPG* novoEstado = new EstadoItemRPG;

    novoEstado->referencia = item;
    novoEstado->desenho.setRadius(7.0f);
    novoEstado->efeito = efeito;
    novoEstado->avisoMostrado = false;
    novoEstado->proximo = estadosItensRPG;

    novoEstado->desenho.setPosition(posicao);

    if (efeito == EfeitoItemRPG::POCAO) {
        novoEstado->desenho.setFillColor(sf::Color::Red);
    } else if (efeito == EfeitoItemRPG::ARMADURA) {
        novoEstado->desenho.setFillColor(sf::Color::Cyan);
    } else {
        novoEstado->desenho.setFillColor(sf::Color::Yellow);
    }

    estadosItensRPG = novoEstado;

    return novoEstado;
}

// Define a posicao de um item que ja existe.
void definirPosicaoItemRPG(Item* item, sf::Vector2f posicao) {
    if (item == nullptr) {
        return;
    }

    EstadoItemRPG* estado =
        obterEstadoItemRPG(item, posicao);

    estado->desenho.setPosition(posicao);
}

// Cria um item usando a funcao do grupo e define sua posicao.
void adicionarItemComPosicaoRPG(
    Sala* sala,
    const std::string& nome,
    int atributo,
    sf::Vector2f posicao,
    EfeitoItemRPG efeito = EfeitoItemRPG::COMUM
) {
    adicionarItem(sala, nome, atributo);

    obterEstadoItemRPG(sala->itens, posicao, efeito);
}

// Coloca o item coletado no inventario e aplica seu efeito.
bool receberItemRPG(
    JogadorRPG& jogador,
    Item* item,
    EstadoItemRPG* estado
) {
    // Impede coletar outra armadura enquanto ja possui uma.
    if (estado->efeito == EfeitoItemRPG::ARMADURA &&
        jogador.armaduraColetada) {
    cout << "\nVoce ja coletou uma armadura.\n";
        return false;
    }

    adicionarAoInventarioRPG(jogador, item->nome, item->atributo);

    // Se for pocao, tambem coloca na pilha.
    if (estado->efeito == EfeitoItemRPG::POCAO) {
        int valorCura = item->atributo;

        if (valorCura <= 0) {
            valorCura = jogador.vidaMax;
        }

        empilharPocaoRPG(jogador, item->nome, valorCura);
        cout << "\nPocao guardada no inventario.\n";
    }

    // Se for armadura, equipa no jogador.
 else if (estado->efeito == EfeitoItemRPG::ARMADURA) {
        jogador.armaduraColetada = true;
        jogador.resistenciaArmadura = item->atributo;

if (jogador.resistenciaArmadura <= 0 || jogador.resistenciaArmadura > jogador.resistenciaMax) {
jogador.resistenciaArmadura = jogador.resistenciaMax;
 }
cout << "\nArmadura equipada.\n";
    }

    // Itens comuns apenas ficam guardados.
else {
cout << "\nItem coletado: " << item->nome << '\n';
}
    return true;
}

// Retira o item da sala e apaga seus dados visuais.
void removerItemDaSalaRPG(
    Sala* sala,
    Item* item,
    Item* itemAnterior
) {
    if (itemAnterior == nullptr) {
        sala->itens = item->proximo;
    } else {
        itemAnterior->proximo = item->proximo;
    }

    EstadoItemRPG* atual = estadosItensRPG;
    EstadoItemRPG* anterior = nullptr;

    while (atual != nullptr) {
    if (atual->referencia == item) {
   if (anterior == nullptr) {
    estadosItensRPG = atual->proximo;
} else {
     anterior->proximo = atual->proximo;
 }
 delete atual;
 break;
 }
   anterior = atual;
   atual = atual->proximo;
}
delete item;
}

// Define posicoes automaticas quando o grupo nao informou uma.
sf::Vector2f posicaoPadraoItemRPG(int numero) {
    float x = 80.0f + (numero % 5) * 80.0f;
    float y = 75.0f + (numero / 5) * 55.0f;
    return sf::Vector2f(x, y);
}

// Desenha os itens que estao na sala.
void desenharItensSalaRPG(sf::RenderWindow& janela, Sala* sala) {
    if (sala == nullptr) return;
    int numero = 0;
    Item* atual = sala->itens;

while (atual != nullptr){
    sf::Vector2f posicao = posicaoPadraoItemRPG(numero);
    EstadoItemRPG* estado = obterEstadoItemRPG(atual, posicao);

if (estado->sprite.has_value()) {
            estado->sprite->setPosition(estado->desenho.getPosition());
            janela.draw(*estado->sprite);
        } else {
            janela.draw(estado->desenho);
        }

        janela.draw(estado->desenho);
        atual = atual->proximo;
        numero++;
}
}

// Verifica se a tecla acabou de ser pressionada.
bool pressionouTeclaRPG(
    sf::Keyboard::Key tecla,
    bool& teclaAnterior
) {
    bool teclaAtual = sf::Keyboard::isKeyPressed(tecla);
    if (teclaAtual && !teclaAnterior) {
        teclaAnterior = true;
        return true;
}
    teclaAnterior = teclaAtual;
    return false;
}

// Procura o item mais proximo do jogador e permite coletar ou recusar.
void atualizarColetaItensRPG(JogadorRPG& jogador, Sala* sala) {
    bool apertouE = pressionouTeclaRPG(sf::Keyboard::Key::E, jogador.teclaEAnterior);
    bool apertouQ = pressionouTeclaRPG(sf::Keyboard::Key::Q, jogador.teclaQAnterior );
    sf::Vector2f posJogador = jogador.corpo.getPosition();
    sf::Vector2f tamanhoJogador = jogador.corpo.getSize();

    // Calcula o centro do jogador.
    float centroX = posJogador.x + tamanhoJogador.x / 2;
    float centroY = posJogador.y + tamanhoJogador.y / 2;

    Item* itemMaisProximo = nullptr;
    Item* itemAnterior = nullptr;
    Item* anterior = nullptr;
    EstadoItemRPG* estadoMaisProximo = nullptr;

    float menorDistancia = 42.0f * 42.0f;
    int numero = 0;
    Item* atual = sala->itens;

    // Percorre todos os itens da sala.
    while (atual != nullptr) {
 sf::Vector2f posItem = posicaoPadraoItemRPG(numero);
        EstadoItemRPG* estado =
            obterEstadoItemRPG(atual, posItem);

        posItem = estado->desenho.getPosition();
        float itemX = posItem.x + 7.0f;
        float itemY = posItem.y + 7.0f;
        float diferencaX = centroX - itemX;
        float diferencaY = centroY - itemY;
        float distancia = diferencaX * diferencaX +diferencaY * diferencaY;

 if (distancia < menorDistancia) {
    menorDistancia = distancia;
    itemMaisProximo = atual;
    itemAnterior = anterior;
    estadoMaisProximo = estado;
} 
 else {
    // Se o jogador se afastar, permite mostrar o aviso novamente.
     estado->avisoMostrado = false;
}
        anterior = atual;
        atual = atual->proximo;
        numero++;
    }

if (itemMaisProximo == nullptr) {
    return;
}

 // Mostra a mensagem apenas uma vez enquanto o jogador estiver perto.
if (!estadoMaisProximo->avisoMostrado) {
cout << "\nItem proximo: " << itemMaisProximo->nome << "\nE - Coletar" << "\nQ - Recusar\n";
        estadoMaisProximo->avisoMostrado = true;
}

// Tecla E: coleta o item.
    if (apertouE) {
    bool conseguiuColetar = receberItemRPG(jogador, itemMaisProximo, estadoMaisProximo);

if (conseguiuColetar) {
    removerItemDaSalaRPG(sala, itemMaisProximo, itemAnterior);
    }
    }

    // Tecla Q: recusa e remove o item.
    else if (apertouQ) {
    cout << "\nItem recusado: "<< itemMaisProximo->nome << '\n';
        jogador.itensRecusados++;

removerItemDaSalaRPG( sala, itemMaisProximo, itemAnterior);
    }
}

// ============================================================
// 5. TUTORIAL, CONTROLES E MOVIMENTACAO DO JOGADOR
// ============================================================

void mostrarTutorialRPG() {
    cout << "\n========== BEM-VINDO AO RPG! ==========\n";
    cout << "WASD ou setas - Movimentar\n";
    cout << "F - Trocar de sala\n";
    cout << "E - Coletar item\n";
    cout << "Q - Recusar item\n";
    cout << "I - Abrir ou fechar inventario\n";
    cout << "H - Usar pocao\n";
    cout << "B - Abrir bestiario\n";
    cout << "ESPACO - Atacar monstro proximo\n";
    cout << "Vida inicial: 100/100\n";
    cout << "Armadura inicial: 0/100\n";
    cout << "Objetivo: encontrar tres chaves e chegar ao Boss.\n";
    cout << "=======================================\n\n";
}

// Verifica os botoes do inventario e da coleta.
void atualizarControlesInventarioRPG(
    JogadorRPG& jogador,
    Sala* sala
) {
    // Tecla I abre ou fecha o inventario.
    if (pressionouTeclaRPG(
        sf::Keyboard::Key::I, jogador.teclaIAnterior
    )) {
        jogador.inventarioAberto = !jogador.inventarioAberto;

        if (jogador.inventarioAberto) {
            mostrarInventarioRPG(jogador);
        } else {
cout << "\nInventario fechado.\n";
        }
    }

    // So permite pegar itens quando o inventario esta fechado.
    if (!jogador.inventarioAberto) {
        atualizarColetaItensRPG(jogador, sala);
    }

    // Tecla H usa uma pocao.
    if (pressionouTeclaRPG(
        sf::Keyboard::Key::H, jogador.teclaHAnterior
    )) {
        usarPocaoRPG(jogador);
    }
}

// Movimenta o jogador e informa se ele atacou.
bool atualizarMovimentoJogadorRPG(
    JogadorRPG& jogador,
    float tempo
) {
    jogador.tempoVulneravel -= tempo;
    jogador.tempoAtaque -= tempo;

    if (jogador.tempoVulneravel < 0) {
        jogador.tempoVulneravel = 0;
    }

    if (jogador.tempoAtaque < 0) {
        jogador.tempoAtaque = 0;
    }

    // Nao movimenta enquanto o inventario estiver aberto.
    if (!jogador.inventarioAberto) {
        sf::Vector2f movimento(0.0f, 0.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            movimento.y = movimento.y - 1;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            movimento.y = movimento.y + 1;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            movimento.x = movimento.x - 1;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            movimento.x = movimento.x + 1;
        }

        // Calcula o tamanho do movimento para evitar diagonal mais rapida.
        float tamanho = std::sqrt(
            movimento.x * movimento.x +
            movimento.y * movimento.y
        );

        if (tamanho > 0) {
            movimento.x = movimento.x / tamanho;
            movimento.y = movimento.y / tamanho;
        }

        sf::Vector2f posicao = jogador.corpo.getPosition();
        sf::Vector2f tamanhoJogador = jogador.corpo.getSize();

        posicao.x += movimento.x * jogador.velocidade * tempo;
        posicao.y += movimento.y * jogador.velocidade * tempo;

        // Impede o jogador de atravessar as paredes.
        if (posicao.x < 32) {
            posicao.x = 32;
        }

        if (posicao.x + tamanhoJogador.x > 480) {
            posicao.x = 480 - tamanhoJogador.x;
        }

        if (posicao.y < 32) {
            posicao.y = 32;
        }

        if (posicao.y + tamanhoJogador.y > 224) {
            posicao.y = 224 - tamanhoJogador.y;
        }

        jogador.corpo.setPosition(posicao);
    }

    // Tecla ESPACO realiza o ataque.
    bool atacou = pressionouTeclaRPG(
        sf::Keyboard::Key::Space,
        jogador.teclaEspacoAnterior
    );

    if (atacou && jogador.tempoAtaque == 0) {
        jogador.tempoAtaque = 0.35f;
        return true;
    }

    return false;
}

// ============================================================
// 6. MOVIMENTACAO DOS MONSTROS E COMBATE
// ============================================================

// Guarda a posicao e o desenho de cada monstro.
struct EstadoMonstroRPG {
    Monstro* referencia;
    sf::RectangleShape corpo;
    sf::Vector2f direcao;
    float tempoParaNovaDirecao;
    float velocidade;
    EstadoMonstroRPG* proximo;
};

EstadoMonstroRPG* estadosMonstrosRPG = nullptr;

// Procura o estado visual do monstro ou cria um novo.
EstadoMonstroRPG* obterEstadoMonstroRPG(
    Monstro* monstro,
    bool ehBoss
) {
    EstadoMonstroRPG* atual = estadosMonstrosRPG;

    while (atual != nullptr) {
        if (atual->referencia == monstro) {
            return atual;
        }

        atual = atual->proximo;
    }

    EstadoMonstroRPG* novoEstado = new EstadoMonstroRPG;

    novoEstado->referencia = monstro;

    if (ehBoss) {
        novoEstado->corpo.setSize(sf::Vector2f(90.0f, 90.0f));
        novoEstado->corpo.setFillColor(sf::Color::Red);
        novoEstado->velocidade = 95.0f;
    } else {
        novoEstado->corpo.setSize(sf::Vector2f(40.0f, 40.0f));
        novoEstado->corpo.setFillColor(sf::Color(150, 50, 50));
        novoEstado->velocidade = 65.0f;
    }

    novoEstado->corpo.setPosition(sf::Vector2f(350.0f, 105.0f));
    novoEstado->direcao = sf::Vector2f(1.0f, 0.0f);
    novoEstado->tempoParaNovaDirecao = 0.0f;

    novoEstado->proximo = estadosMonstrosRPG;
    estadosMonstrosRPG = novoEstado;

    return novoEstado;
}

// Faz o monstro andar e mudar de direcao aleatoriamente.
void movimentarMonstroRPG(EstadoMonstroRPG& monstro,float tempo, mt19937& gerador) {
    monstro.tempoParaNovaDirecao -= tempo;

    // Escolhe uma nova direcao aleatoria.
    if (monstro.tempoParaNovaDirecao <= 0) {
        uniform_int_distribution<int> distribuicao(-1, 1);

        int x = distribuicao(gerador);
        int y = distribuicao(gerador);

        if (x == 0 && y == 0) {
            x = 1;
        }

        float tamanho = std::sqrt(
            static_cast<float>(x * x + y * y)
        );

        monstro.direcao.x = x / tamanho;
        monstro.direcao.y = y / tamanho;

        monstro.tempoParaNovaDirecao = 0.8f;
    }

    sf::Vector2f posicao = monstro.corpo.getPosition();
    sf::Vector2f tamanhoMonstro = monstro.corpo.getSize();

    posicao.x += monstro.direcao.x * monstro.velocidade * tempo;
    posicao.y += monstro.direcao.y * monstro.velocidade * tempo;

    // Impede que o monstro atravesse as paredes.
    if (posicao.x < 32 ||
        posicao.x + tamanhoMonstro.x > 480) {

        monstro.direcao.x *= -1;

        if (posicao.x < 32) {
            posicao.x = 32;
        } else {
            posicao.x = 480 - tamanhoMonstro.x;
        }
    }

    if (posicao.y < 32 ||
        posicao.y + tamanhoMonstro.y > 224) {

        monstro.direcao.y *= -1;

        if (posicao.y < 32) {
            posicao.y = 32;
        } else {
            posicao.y = 224 - tamanhoMonstro.y;
        }
    }

    monstro.corpo.setPosition(posicao);
}


// Verifica se dois retangulos estao encostando.
bool detectarColisaoAABBRPG(
    const sf::RectangleShape& primeiro,
    const sf::RectangleShape& segundo
) {
    sf::Vector2f posA = primeiro.getPosition();
    sf::Vector2f tamA = primeiro.getSize();

    sf::Vector2f posB = segundo.getPosition();
    sf::Vector2f tamB = segundo.getSize();

    bool colisaoX = posA.x < posB.x + tamB.x &&
                    posA.x + tamA.x > posB.x;

    bool colisaoY = posA.y < posB.y + tamB.y &&
                    posA.y + tamA.y > posB.y;

    return colisaoX && colisaoY;
}

// Aplica o dano primeiro na armadura e depois na vida.
void receberDanoJogadorRPG(JogadorRPG& jogador, int dano) {
    if (jogador.vida <= 0 || jogador.tempoVulneravel > 0) {
        return;
    }

    dano = max(0, dano);
    int danoArmadura = min(dano, jogador.resistenciaArmadura);

    jogador.resistenciaArmadura -= danoArmadura;
    jogador.vida = max(0, jogador.vida - (dano - danoArmadura));
    jogador.tempoVulneravel = 0.8f;

    if (jogador.resistenciaArmadura == 0) {
        jogador.armaduraColetada = false;
        if (danoArmadura > 0) {
            cout << "\nSua armadura quebrou!\n";
        }
    }

    cout << "\nVoce recebeu um golpe.\n";
    cout << "Dano na vida: " << dano - danoArmadura << '\n';
}

// Atualiza o monstro, verifica ataques e verifica colisao.
void atualizarCombateSalaRPG(JogadorRPG& jogador,Sala* sala,float tempo,bool jogadorAtacou,mt19937& gerador) 
{
    // O parametro gerador e mantido para continuar compativel
    // com a chamada que ja existe no main do projeto.
    if (sala == nullptr ||
        sala->monstro == nullptr ||
        sala->monstro->vida <= 0) {
        return;
    }

    Monstro* monstro = sala->monstro;

    EstadoMonstroRPG* estado = obterEstadoMonstroRPG(monstro, sala->ehBoss);

    movimentarMonstroRPG(*estado, tempo, gerador);

    // Verifica se o jogador acertou o monstro.
    if (jogadorAtacou) {
        sf::Vector2f posJogador = jogador.corpo.getPosition();
        sf::Vector2f tamJogador = jogador.corpo.getSize();
        sf::Vector2f posMonstro = estado->corpo.getPosition();
        sf::Vector2f tamMonstro = estado->corpo.getSize();

    float centroJogadorX = posJogador.x + tamJogador.x / 2;
    float centroJogadorY = posJogador.y + tamJogador.y / 2;
    float centroMonstroX = posMonstro.x + tamMonstro.x / 2;
    float centroMonstroY = posMonstro.y + tamMonstro.y / 2;
    float diferencaX = centroJogadorX - centroMonstroX;
    float diferencaY = centroJogadorY - centroMonstroY;
    float distancia = diferencaX * diferencaX + diferencaY * diferencaY;

    if (distancia <= 55.0f * 55.0f) {
         monstro->vida -= 15;

    if (monstro->vida < 0) {
        monstro->vida = 0;
   }

cout << "\nVoce atingiu " << monstro->nome << ".\n";
cout << "Vida restante: " << monstro->vida << '\n';

if (monstro->vida == 0) {
cout << monstro->nome << " foi derrotado!\n";
 return;
}
}
}

// Se o jogador encostar no monstro, recebe dano.
if (detectarColisaoAABBRPG(jogador.corpo, estado->corpo)) {
    int dano = monstro->ataque;

// Itens recusados aumentam o dano do Boss.
        if (sala->ehBoss) {
            dano += jogador.itensRecusados * 2;
        }
        receberDanoJogadorRPG(jogador, dano);
 }
}

// Desenha o monstro da sala.
void desenharMonstroSalaRPG(
    sf::RenderWindow& janela,
    Sala* sala
) {
    if (sala == nullptr ||
    sala->monstro == nullptr ||
    sala->monstro->vida <= 0) {
        return;
}

    EstadoMonstroRPG* estado =
    obterEstadoMonstroRPG(sala->monstro, sala->ehBoss);
    janela.draw(estado->corpo);
}

// ============================================================
// 7. BARRAS DE VIDA E ARMADURA
// ============================================================

// Desenha uma barra de acordo com o valor atual.
void desenharBarraRPG(sf::RenderWindow& janela,sf::Vector2f posicao,
    float largura,
    float altura,
    int valor,
    int valorMaximo,
    sf::Color cor) {
    float porcentagem = 0.0f;
    if (valorMaximo > 0) {
        porcentagem = static_cast<float>(valor) / valorMaximo;
    }

    if (porcentagem < 0) {
        porcentagem = 0;
    }

    if (porcentagem > 1) {
        porcentagem = 1;
    }

    sf::RectangleShape fundo;
    fundo.setSize(sf::Vector2f(largura, altura));
    fundo.setPosition(posicao);
    fundo.setFillColor(sf::Color(45, 45, 45));

    sf::RectangleShape preenchimento;
    preenchimento.setSize(sf::Vector2f(largura * porcentagem, altura));
    preenchimento.setPosition(posicao);
    preenchimento.setFillColor(cor);

    janela.draw(fundo);
    janela.draw(preenchimento);
}

// Desenha as duas barras acima do jogador.
void desenharBarrasJogadorRPG(
    sf::RenderWindow& janela,
    const JogadorRPG& jogador
) {
    sf::Vector2f posicao = jogador.corpo.getPosition();
    sf::Vector2f tamanho = jogador.corpo.getSize();
    float x = posicao.x + tamanho.x / 2 - 17;

    desenharBarraRPG(
        janela,
        sf::Vector2f(x, posicao.y - 13),
        34, 4,
        jogador.vida,
        jogador.vidaMax,
        sf::Color::Green
    );

    desenharBarraRPG(
        janela,
        sf::Vector2f(x, posicao.y - 7),
        34, 4,
        jogador.resistenciaArmadura,
        jogador.resistenciaMax,
        sf::Color::Cyan
    );
}

// Atualiza o inventario quando uma chave e coletada no mapa
void sincronizarChavesInventarioRPG(JogadorRPG& jogador, int chavesColetadas) {
    while (jogador.chavesRegistradas < chavesColetadas) {
    adicionarAoInventarioRPG(jogador, "Chave", 0);
        jogador.chavesRegistradas++;
    }
}

// ============================================================
//  LIBERACAO DA MEMORIA DA PARTE DO JOGADOR
// ============================================================

// Apaga os elementos criados dinamicamente pelo modulo do jogador
// Nao apaga os itens e monstros do mapa: essa tarefa continua sendo
// feita pela funcao liberarMapa() 
void liberarModuloJogadorRPG(JogadorRPG& jogador) {
    while (jogador.inventario != nullptr) {
        NoInventarioRPG* apagar = jogador.inventario;
        jogador.inventario = jogador.inventario->proximo;
    delete apagar;
    }

while (jogador.topoPocoes != nullptr) {
        NoPocaoRPG* apagar = jogador.topoPocoes;
    jogador.topoPocoes = jogador.topoPocoes->proximo;
        delete apagar;
    }

while (estadosItensRPG != nullptr) {
EstadoItemRPG* apagar = estadosItensRPG;
estadosItensRPG = estadosItensRPG->proximo;
    delete apagar;
    }

while (estadosMonstrosRPG != nullptr) {
    EstadoMonstroRPG* apagar = estadosMonstrosRPG;
    estadosMonstrosRPG = estadosMonstrosRPG->proximo;
delete apagar;
}
}
////////// final - Emily /////////////////

#include "batalha.hpp" //arquivo da tela de batalha
#include "bestiario.hpp"

void depurarMonstrosRPG() {
    for (NoSala* n = listaSalas; n != nullptr; n = n->proximo) {
        Sala* s = n->sala;
        cout << s->nome << ": ";
        if (s->monstro == nullptr) cout << "SEM monstro\n";
        else cout << s->monstro->nome << " vida=" << s->monstro->vida
                  << " ataque=" << s->monstro->ataque << '\n';
    }
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

    adicionarMonstro(boss, "Chefão", 120, 15);
    
    // Monstros das salas 2 e 3 (sem eles essas salas nao dao chave)
adicionarMonstro(sala1, "Esqueleto", 40, 8);
adicionarMonstro(sala2, "Slime Azul", 60, 10);
adicionarMonstro(sala3, "Morcego", 50, 9);

// Recompensa de cada sala: chave + item
definirRecompensaRPG(sala1, "Espada", 10, EfeitoItemRPG::COMUM);
definirRecompensaRPG(sala2, "Armadura de Ferro", 50, EfeitoItemRPG::ARMADURA);
definirRecompensaRPG(sala3, "Pocao de Cura", 40, EfeitoItemRPG::POCAO);

preencherBestiarioRPG();

depurarMonstrosRPG();


// ==================================================
// INICIALIZACAO DA PARTE DO JOGADOR - EMily
// ==================================================

// Cria o jogador
JogadorRPG jogador;

// Define a sala inicial
Sala* salaAtual = entrada;

// Controla a quantidade de chaves
int chavesColetadas = 0;

// Controla o tempo do jogo
Clock relogio;

// Gerador de numeros aleatorios
mt19937 gerador(random_device{}());

// Controle da tecla F
bool teclaFAntes = false;

// Exibe o tutorial
mostrarTutorialRPG();

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

    Texture texBoss, texSkeleto, texSlimeAzul, texbat;
    definirSpriteMonstroRPG(sala1, texSkeleto, "img/native/monsters/skeleton.png");
    definirSpriteMonstroRPG(sala2, texSlimeAzul, "C:\\Users\\rilso\\OneDrive\\Área de Trabalho\\trabalho final\\Trabalho-Final-prog-2\\img\\pixel art\\slime.png");
    definirSpriteMonstroRPG(sala3, texbat, "C:\\Users\\rilso\\OneDrive\\Área de Trabalho\\trabalho final\\Trabalho-Final-prog-2\\img\\pixel art\\morcego.png");
    definirSpriteMonstroRPG(boss, texBoss, "C:\\Users\\rilso\\OneDrive\\Área de Trabalho\\trabalho final\\Trabalho-Final-prog-2\\img\\pixel art\\slimered.png");

// ==================================================
// PIXEL ART DO JOGADOR CONFIG- Marya
// ==================================================
Texture texturaJogador;
if (!texturaJogador.loadFromFile("img/personagem.png")) { // <-- Coloque o caminho da sua imagem aqui
    cout << "Erro ao carregar a imagem do personagem!\n";
    return 1;
}
texturaJogador.setSmooth(false); // Mantem a pixel art nitida
//Ajuste automático de tamanho
Vector2u tamImg = texturaJogador.getSize();
jogador.sprite.emplace(texturaJogador);
jogador.sprite->setScale({
    40.0f / static_cast<float>(tamImg.x),
    40.0f / static_cast<float>(tamImg.y)
});

//==================================================
// PIXEL ART ITENS CONFIG- Marya
//==================================================
Texture texPocao, texArmadura, texItemComum;

if (texPocao.loadFromFile("C:\\Users\\rilso\\OneDrive\\Área de Trabalho\\trabalho final\\Trabalho-Final-prog-2\\img\\pixel art\\poção.png")) texPocao.setSmooth(false);
if (texArmadura.loadFromFile("C:\\Users\\rilso\\OneDrive\\Área de Trabalho\\trabalho final\\Trabalho-Final-prog-2\\img\\pixel art\\arm.png")) texArmadura.setSmooth(false);
if (texItemComum.loadFromFile("C:\\Users\\rilso\\OneDrive\\Área de Trabalho\\trabalho final\\Trabalho-Final-prog-2\\img\\pixel art\\espada.png")) texItemComum.setSmooth(false);

aplicarTexturasItensRPG(texPocao, texArmadura, texItemComum);
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

        //emily//
// Calcula o tempo entre os quadros
float dt = relogio.restart().asSeconds();

if (processarBatalhaRPG(jogador, salaAtual, dt, gerador, janela, chavesColetadas)) continue;

// Movimentacao do jogador
bool ataque = atualizarMovimentoJogadorRPG(jogador, dt);

// Controles do inventario, coleta e pocoes
atualizarControlesInventarioRPG(jogador, salaAtual);

atualizarBestiarioRPG();

if (verificarEncontroRPG(jogador, salaAtual)) continue;

// Atualiza o combate e a movimentacao do monstro
atualizarCombateSalaRPG(jogador,salaAtual,dt,false,gerador);
if (jogador.vida <= 0) {
    jogador.vida = 0;

    cout << "\n====================" << endl;
    cout << "      GAME OVER      " << endl;
    cout << "Voce ficou sem vida!" << endl;
    cout << "====================" << endl;

    janela.close();
    break;
}

// Atualiza as chaves no inventario
sincronizarChavesInventarioRPG( jogador,chavesColetadas);

// Troca de sala utilizando a tecla F
bool teclaFAtual =
    Keyboard::isKeyPressed(Keyboard::Key::F);

if (teclaFAtual && !teclaFAntes) {
    mostrarSaidas(salaAtual);

    if (!moverJogador(salaAtual, chavesColetadas)) {
        janela.close();
    }

    // Atualiza as chaves do inventario
    sincronizarChavesInventarioRPG(jogador,chavesColetadas
    );

    // Reposiciona o jogador ao entrar em outra sala
    jogador.corpo.setPosition({245.0f, 120.0f});
}
teclaFAntes = teclaFAtual;
//final - emily



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
// Desenha os itens da sala atual
desenharItensSalaRPG(janela, salaAtual);

// Desenha o monstro da sala atual
desenharMonstroSalaRPG(janela, salaAtual);

// Desenha o jogador
if (jogador.sprite.has_value()) {
    jogador.sprite->setPosition(jogador.corpo.getPosition());
    janela.draw(*jogador.sprite);
} else {
    janela.draw(jogador.corpo);
}

// Desenha as barras de vida e armadura
desenharBarrasJogadorRPG(janela, jogador);

        // Exibe tudo o que foi desenhado
        janela.display();
    }
    // Libera a memoria da parte do jogador
    liberarModuloJogadorRPG(jogador);

    // Libera a memoria depois que a janela fecha
    liberarMapa();

    return 0;
}
