// ============================================================================
// bestiario.hpp - BESTIARIO DO JOGO (cobre R8 e R10 do checklist)
// ============================================================================
// O bestiario e o catalogo dos monstros do jogo. Aperte B no mapa para abrir
// o menu no console:
//
//   1 - Listar em ordem alfabetica      -> R8  (travessia in-ordem da ABB)
//   2 - Buscar um monstro pelo nome     -> R8 + R10 (ABB, sequencial, binaria)
//   3 - Comparar as buscas              -> R10 (contagem de comparacoes e tempo)
//
// Estruturas:
//   - ABB (arvore binaria de busca) indexada pelo NOME do monstro.
//   - vector ordenado: copia dos dados usada pelas buscas.
//
// Por que usar uma ABB: a travessia in-ordem entrega os monstros ja em ordem
// alfabetica, sem precisar de algoritmo de ordenacao (acesso ordenado aos
// dados). O R9 (ordenacao) ja e coberto pelo Selection Sort do inventario
// (ordenarInventarioRPG), entao nao ha outro algoritmo de ordenacao aqui.
//
// Incluir DEPOIS de batalha.hpp (precisa de Monstro, Sala, NoSala, listaSalas
// e pressionouTeclaRPG, que ja existem acima).
// ============================================================================
#pragma once

#include <vector>
#include <chrono>
#include <iomanip>

// ---------- Estruturas ----------

// No da arvore binaria de busca.
struct NoBestiarioRPG {
    string nome;
    int vidaMax;
    int ataque;
    bool ehBoss;
    Monstro* referencia;          // monstro real do mapa (para saber se morreu)
    NoBestiarioRPG* esquerda;
    NoBestiarioRPG* direita;
};

// Copia "achatada" de um no, usada nos vetores (buscas e listagem).
struct EntradaBestiarioRPG {
    string nome;
    int vidaMax = 0;
    int ataque = 0;
    bool ehBoss = false;
    bool derrotado = false;
};

NoBestiarioRPG* raizBestiarioRPG = nullptr;

// ============================================================================
// R8 - ARVORE BINARIA DE BUSCA (chave: nome do monstro)
// ============================================================================
// Complexidade:
//   - Insercao e busca: O(h), onde h e a altura da arvore.
//       Caso medio (arvore equilibrada): h ~ log2(n)  -> O(log n)
//       Pior caso (nomes inseridos ja em ordem): h = n -> O(n)
//   - Travessia in-ordem: O(n), visita cada no uma vez, em ordem alfabetica.
//   - Espaco: O(h) de pilha de recursao.
// Nomes repetidos sao ignorados (cada nome aparece uma vez no bestiario).

// Insere um monstro na arvore (recursivo). Devolve a raiz da subarvore.
NoBestiarioRPG* inserirBestiarioRPG(NoBestiarioRPG* raiz, Monstro* monstro, bool ehBoss) {
    if (raiz == nullptr) {
        NoBestiarioRPG* novo = new NoBestiarioRPG;
        novo->nome = monstro->nome;
        novo->vidaMax = monstro->vida;
        novo->ataque = monstro->ataque;
        novo->ehBoss = ehBoss;
        novo->referencia = monstro;
        novo->esquerda = nullptr;
        novo->direita = nullptr;
        return novo;
    }

    if (monstro->nome < raiz->nome) {
        raiz->esquerda = inserirBestiarioRPG(raiz->esquerda, monstro, ehBoss);
    } else if (monstro->nome > raiz->nome) {
        raiz->direita = inserirBestiarioRPG(raiz->direita, monstro, ehBoss);
    }
    // nome igual: ignora

    return raiz;
}

// Busca um nome na arvore (recursivo). Conta as comparacoes feitas.
NoBestiarioRPG* buscarBestiarioRPG(
    NoBestiarioRPG* raiz,
    const string& nome,
    long long& comparacoes
) {
    if (raiz == nullptr) {
        return nullptr;
    }

    comparacoes++;

    if (nome == raiz->nome) {
        return raiz;
    }

    if (nome < raiz->nome) {
        return buscarBestiarioRPG(raiz->esquerda, nome, comparacoes);
    }

    return buscarBestiarioRPG(raiz->direita, nome, comparacoes);
}

// Travessia IN-ORDEM: esquerda, no, direita.
// Preenche o vetor ja em ordem alfabetica.
void percorrerEmOrdemBestiarioRPG(
    NoBestiarioRPG* raiz,
    vector<EntradaBestiarioRPG>& saida
) {
    if (raiz == nullptr) {
        return;
    }

    percorrerEmOrdemBestiarioRPG(raiz->esquerda, saida);

    EntradaBestiarioRPG entrada;
    entrada.nome = raiz->nome;
    entrada.vidaMax = raiz->vidaMax;
    entrada.ataque = raiz->ataque;
    entrada.ehBoss = raiz->ehBoss;
    entrada.derrotado = (raiz->referencia != nullptr && raiz->referencia->vida <= 0);
    saida.push_back(entrada);

    percorrerEmOrdemBestiarioRPG(raiz->direita, saida);
}

// Libera a arvore inteira (pos-ordem: filhos antes do pai).
void liberarBestiarioRPG(NoBestiarioRPG* raiz) {
    if (raiz == nullptr) {
        return;
    }

    liberarBestiarioRPG(raiz->esquerda);
    liberarBestiarioRPG(raiz->direita);
    delete raiz;
}

// Libera a arvore automaticamente quando o programa fecha.
struct GuardaBestiarioRPG {
    ~GuardaBestiarioRPG() {
        liberarBestiarioRPG(raizBestiarioRPG);
        raizBestiarioRPG = nullptr;
    }
};
GuardaBestiarioRPG guardaBestiarioRPG;

// Cadastra no bestiario todos os monstros que existem nas salas.
// Chamar uma vez no main, depois de criar os monstros.
void preencherBestiarioRPG() {
    NoSala* no = listaSalas;

    while (no != nullptr) {
        Sala* sala = no->sala;

        if (sala->monstro != nullptr) {
            raizBestiarioRPG = inserirBestiarioRPG(raizBestiarioRPG, sala->monstro, sala->ehBoss);
        }
        no = no->proximo;
    }
}

// ============================================================================
// R10 - BUSCA SEQUENCIAL E BUSCA BINARIA (sobre vetor ordenado por nome)
// ============================================================================
// Contagem: uma comparacao por elemento examinado.
//
// Busca sequencial: percorre do inicio ao fim.
//   - Melhor caso O(1), pior caso e caso medio O(n).
//   - Funciona em vetor desordenado.
//
// Busca binaria: compara com o elemento do meio e descarta metade do vetor.
//   - Melhor caso O(1), pior caso e caso medio O(log2 n).
//   - EXIGE vetor ordenado (o vetor vem da travessia in-ordem da ABB, ja
//     ordenado por nome).

// Devolve a posicao do nome, ou -1 se nao existir.
int buscaSequencialRPG(
    const vector<EntradaBestiarioRPG>& vetor,
    const string& nome,
    long long& comparacoes
) {
    for (size_t i = 0; i < vetor.size(); i++) {
        comparacoes++;

        if (vetor[i].nome == nome) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int buscaBinariaRPG(
    const vector<EntradaBestiarioRPG>& vetor,
    const string& nome,
    long long& comparacoes
) {
    int inicio = 0;
    int fim = static_cast<int>(vetor.size()) - 1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        comparacoes++;

        if (vetor[meio].nome == nome) {
            return meio;
        }

        if (nome < vetor[meio].nome) {
            fim = meio - 1;
        } else {
            inicio = meio + 1;
        }
    }
    return -1;
}

// ============================================================================
// DEMONSTRACAO COMPARATIVA (opcao 3 do menu)
// ============================================================================

// Nomes com zeros a esquerda ("Monstro00042"): gerados em ordem crescente,
// ja ficam em ordem alfabetica, entao o vetor ja nasce ordenado.
string gerarNomeTesteRPG(int numero) {
    string texto = to_string(numero);

    while (texto.size() < 5) {
        texto = "0" + texto;
    }
    return "Monstro" + texto;
}

// Mede o tempo medio (em microssegundos) de uma busca repetida varias vezes.
double medirTempoBuscaRPG(
    const vector<EntradaBestiarioRPG>& vetor,
    const string& nome,
    bool binaria,
    int repeticoes
) {
    volatile int guarda = 0; // evita que o compilador apague o laco
    long long contador = 0;

    auto inicio = chrono::steady_clock::now();

    for (int i = 0; i < repeticoes; i++) {
        if (binaria) {
            guarda = guarda + buscaBinariaRPG(vetor, nome, contador);
        } else {
            guarda = guarda + buscaSequencialRPG(vetor, nome, contador);
        }
    }

    auto fim = chrono::steady_clock::now();
    double total = chrono::duration<double, micro>(fim - inicio).count();

    return total / repeticoes;
}

void imprimirComparacaoBuscaRPG(
    const vector<EntradaBestiarioRPG>& vetor,
    const string& nome
) {
    const int REPETICOES = 5000;

    long long compSeq = 0;
    long long compBin = 0;
    int posSeq = buscaSequencialRPG(vetor, nome, compSeq);
    int posBin = buscaBinariaRPG(vetor, nome, compBin);

    double tempoSeq = medirTempoBuscaRPG(vetor, nome, false, REPETICOES);
    double tempoBin = medirTempoBuscaRPG(vetor, nome, true, REPETICOES);

    cout << "\nBuscando \"" << nome << "\" ("
         << (posSeq >= 0 ? "existe" : "nao existe") << "):\n";
    cout << "  Sequencial: " << compSeq << " comparacoes | "
         << fixed << setprecision(3) << tempoSeq << " us por busca\n";
    cout << "  Binaria:    " << compBin << " comparacoes | "
         << fixed << setprecision(3) << tempoBin << " us por busca\n";

    if (posSeq != posBin) {
        cout << "  (aviso: as duas buscas deram posicoes diferentes!)\n";
    }

    cout.unsetf(ios::floatfield);
    cout << setprecision(6);
}

void compararBuscasRPG() {
    const int TOTAL = 5000;

    // Gera TOTAL nomes ja em ordem alfabetica (requisito da busca binaria).
    vector<EntradaBestiarioRPG> vetor;
    for (int i = 0; i < TOTAL; i++) {
        EntradaBestiarioRPG e;
        e.nome = gerarNomeTesteRPG(i);
        e.vidaMax = i;
        e.ataque = 1;
        vetor.push_back(e);
    }

    cout << "\n===== COMPARACAO DE BUSCAS (" << TOTAL << " itens) =====\n";

    // Tres casos: primeiro, ultimo (pior caso da sequencial) e inexistente.
    imprimirComparacaoBuscaRPG(vetor, gerarNomeTesteRPG(0));
    imprimirComparacaoBuscaRPG(vetor, gerarNomeTesteRPG(TOTAL - 1));
    imprimirComparacaoBuscaRPG(vetor, "Inexistente");

    cout << "\nConclusao: a sequencial cresce com n (O(n)); a binaria cresce"
         << " com log2(n) (O(log n)).\n";
}

// ============================================================================
// MENU DO BESTIARIO (tecla B)
// ============================================================================

void imprimirEntradaBestiarioRPG(const EntradaBestiarioRPG& e, int posicao) {
    cout << posicao << ". " << (e.ehBoss ? "[BOSS] " : "") << e.nome
         << " | Vida: " << e.vidaMax
         << " | Ataque: " << e.ataque
         << " | Derrotado: " << (e.derrotado ? "Sim" : "Nao") << '\n';
}

// Opcao 1: ordem alfabetica pela travessia in-ordem da ABB.
void listarAlfabeticoBestiarioRPG() {
    vector<EntradaBestiarioRPG> lista;
    percorrerEmOrdemBestiarioRPG(raizBestiarioRPG, lista);

    cout << "\n--- Ordem alfabetica (travessia in-ordem da ABB) ---\n";
    for (size_t i = 0; i < lista.size(); i++) {
        imprimirEntradaBestiarioRPG(lista[i], static_cast<int>(i) + 1);
    }
}

// Opcao 2: busca o mesmo nome pelos tres metodos.
void buscarMonstroBestiarioRPG() {
    cout << "Nome do monstro (exatamente como aparece, ex.: Lobo Mau): ";

    string nome;
    getline(cin >> ws, nome);

    // Vetor ja sai ordenado por nome, pois vem da travessia in-ordem.
    vector<EntradaBestiarioRPG> lista;
    percorrerEmOrdemBestiarioRPG(raizBestiarioRPG, lista);

    long long compArvore = 0;
    long long compSeq = 0;
    long long compBin = 0;

    NoBestiarioRPG* achado = buscarBestiarioRPG(raizBestiarioRPG, nome, compArvore);
    int posSeq = buscaSequencialRPG(lista, nome, compSeq);
    int posBin = buscaBinariaRPG(lista, nome, compBin);

    if (achado == nullptr) {
        cout << "\nMonstro \"" << nome << "\" nao encontrado no bestiario.\n";
    } else {
        cout << "\nEncontrado: " << (achado->ehBoss ? "[BOSS] " : "") << achado->nome
             << " | Vida: " << achado->vidaMax
             << " | Ataque: " << achado->ataque
             << " | Derrotado: "
             << ((achado->referencia != nullptr && achado->referencia->vida <= 0) ? "Sim" : "Nao")
             << '\n';
    }

    cout << "Comparacoes - ABB: " << compArvore
         << " | Sequencial: " << compSeq
         << " | Binaria: " << compBin << '\n';

    if (posSeq != posBin) {
        cout << "(aviso: as buscas deram posicoes diferentes)\n";
    }
}

void abrirBestiarioRPG() {
    int opcao = -1;

    while (opcao != 0) {
        cout << "\n========== BESTIARIO ==========\n";
        cout << "1 - Listar em ordem alfabetica (ABB)\n";
        cout << "2 - Buscar monstro pelo nome\n";
        cout << "3 - Comparar buscas (sequencial x binaria)\n";
        cout << "0 - Voltar ao jogo\n";
        cout << "Escolha: ";

        if (!(cin >> opcao)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Entrada invalida!\n";
            opcao = -1;
            continue;
        }
        if (!(cin >> opcao)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Entrada invalida!\n";
            opcao = -1;
            continue;
        }
        cin.ignore(10000, '\n'); // limpa qualquer sobra na linha apos ler o numero

        if (opcao == 0) {
            break;
        }

        if (raizBestiarioRPG == nullptr && (opcao == 1 || opcao == 2)) {
            cout << "\nBestiario vazio.\n";
            continue;
        }

        if (opcao == 1) {
            listarAlfabeticoBestiarioRPG();
        } else if (opcao == 2) {
            buscarMonstroBestiarioRPG();
        } else if (opcao == 3) {
            compararBuscasRPG();
        } else {
            cout << "Opcao invalida!\n";
        }
    }

    cout << "\nVoltando ao jogo...\n";
}

// Chamar uma vez por quadro no main: abre o bestiario ao apertar B.
void atualizarBestiarioRPG() {
    static bool teclaBAnterior = false;

    if (pressionouTeclaRPG(sf::Keyboard::Key::B, teclaBAnterior)) {
        abrirBestiarioRPG();
    }
}
