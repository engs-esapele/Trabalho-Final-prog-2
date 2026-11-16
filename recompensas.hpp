// ============================================================================
// recompensas.hpp - CHAVE E ITEM AO DERROTAR O MONSTRO DA SALA
// ============================================================================
//
// Cada sala pode ter uma recompensa:
//   - a CHAVE e entregue automaticamente quando o monstro morre
//   - o ITEM (espada, armadura, pocao...) cai no chao, onde o monstro estava,
//     e o jogador coleta com E ou recusa com Q (como os outros itens)
//
// As recompensas ficam numa lista encadeada; cada uma e removida depois
// de entregue.
// ============================================================================
#pragma once

struct RecompensaRPG {
    Sala* sala;
    string nomeItem;
    int atributo;
    EfeitoItemRPG efeito;
    RecompensaRPG* proximo;
};

RecompensaRPG* listaRecompensasRPG = nullptr;

// Registra a recompensa de uma sala.
// Tambem desliga a chave "automatica" da sala (temChave), para ela
// so ser entregue depois de derrotar o monstro.
void definirRecompensaRPG(
    Sala* sala,
    const string& nomeItem,
    int atributo,
    EfeitoItemRPG efeito = EfeitoItemRPG::COMUM
) {
    RecompensaRPG* nova = new RecompensaRPG;

    nova->sala = sala;
    nova->nomeItem = nomeItem;
    nova->atributo = atributo;
    nova->efeito = efeito;
    nova->proximo = listaRecompensasRPG;
    listaRecompensasRPG = nova;

    sala->temChave = false;
}

// Entrega a recompensa da sala (se existir) e remove ela da lista.
void entregarRecompensaRPG(Sala* sala, int& chaves) {
    RecompensaRPG* atual = listaRecompensasRPG;
    RecompensaRPG* anterior = nullptr;

    while (atual != nullptr && atual->sala != sala) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual == nullptr) {
        return;
    }

    // Chave entregue na hora
    chaves++;
    cout << "\nO monstro deixou uma chave! Chaves coletadas: "
         << chaves << "/3\n";

    // Item cai onde o monstro estava
    sf::Vector2f posicao = posicaoPadraoItemRPG(0);

    if (sala->monstro != nullptr) {
        posicao = obterEstadoMonstroRPG(sala->monstro, sala->ehBoss)
                      ->corpo.getPosition();
    }

    adicionarItemComPosicaoRPG(
        sala, atual->nomeItem, atual->atributo, posicao, atual->efeito
    );

    cout << "O monstro tambem deixou no chao: " << atual->nomeItem;

    // Remove o no entregue
    if (anterior == nullptr) {
        listaRecompensasRPG = atual->proximo;
    } else {
        anterior->proximo = atual->proximo;
    }
    delete atual;
}

// Libera as recompensas que nao foram entregues, ao fechar o programa.
struct GuardaRecompensasRPG {
    ~GuardaRecompensasRPG() {
        while (listaRecompensasRPG != nullptr) {
            RecompensaRPG* apagar = listaRecompensasRPG;
            listaRecompensasRPG = listaRecompensasRPG->proximo;
            delete apagar;
        }
    }
};
GuardaRecompensasRPG guardaRecompensasRPG;
