// ============================================================================
// batalha.hpp - TELA DE BATALHA (Isabely)
// ============================================================================
// Arquivo separado para nao mexer no codigo dos outros.
// Estruturas: duas FILAS (FIFO) 
//   - FilaTurnosRPG    : quem age (sai do inicio e volta para o fim)
//   - FilaMensagensRPG : log de combate (mantem as 4 ultimas mensagens)
// ============================================================================
#pragma once

const int ATOR_JOGADOR = 0;
const int ATOR_MONSTRO = 1;
const int MAX_MENSAGENS_RPG = 4;

enum class ResultadoBatalhaRPG {
    NENHUM,
    VITORIA,
    DERROTA,
    FUGA
};

// ---------- Fila de turnos ----------
struct NoTurnoRPG {
    int ator;
    NoTurnoRPG* proximo;
};

struct FilaTurnosRPG {
    NoTurnoRPG* inicio = nullptr;
    NoTurnoRPG* fim = nullptr;
};

// Insere no FIM da fila.
void enfileirarTurnoRPG(FilaTurnosRPG& fila, int ator) {
    NoTurnoRPG* novo = new NoTurnoRPG;
    novo->ator = ator;
    novo->proximo = nullptr;

    if (fila.fim == nullptr) {
        fila.inicio = novo;
        fila.fim = novo;
    } else {
        fila.fim->proximo = novo;
        fila.fim = novo;
    }
}

// Remove do INICIO da fila e devolve o ator (-1 se vazia).
int desenfileirarTurnoRPG(FilaTurnosRPG& fila) {
    if (fila.inicio == nullptr) {
        return -1;
    }

    NoTurnoRPG* removido = fila.inicio;
    int ator = removido->ator;

    fila.inicio = removido->proximo;
    if (fila.inicio == nullptr) {
        fila.fim = nullptr;
    }

    delete removido;
    return ator;
}

// Consulta quem e o proximo sem remover.
int turnoAtualRPG(const FilaTurnosRPG& fila) {
    if (fila.inicio == nullptr) {
        return -1;
    }
    return fila.inicio->ator;
}

// ---------- Fila de mensagens ----------
struct NoMensagemRPG {
    string texto;
    NoMensagemRPG* proximo;
};

struct FilaMensagensRPG {
    NoMensagemRPG* inicio = nullptr;
    NoMensagemRPG* fim = nullptr;
    int tamanho = 0;
};

void enfileirarMensagemRPG(FilaMensagensRPG& fila, const string& texto) {
    NoMensagemRPG* novo = new NoMensagemRPG;
    novo->texto = texto;
    novo->proximo = nullptr;

    if (fila.fim == nullptr) {
        fila.inicio = novo;
        fila.fim = novo;
    } else {
        fila.fim->proximo = novo;
        fila.fim = novo;
    }
    fila.tamanho++;

    // Mantem so as ultimas mensagens: remove a mais antiga.
    while (fila.tamanho > MAX_MENSAGENS_RPG) {
        NoMensagemRPG* removido = fila.inicio;
        fila.inicio = removido->proximo;
        if (fila.inicio == nullptr) {
            fila.fim = nullptr;
        }
        delete removido;
        fila.tamanho--;
    }
}

// ---------- Estado da batalha ----------
struct BatalhaRPG {
    bool ativa = false;
    Monstro* monstro = nullptr;
    bool ehBoss = false;
    int vidaInicialMonstro = 1;   // Monstro nao tem vidaMax; usada na barra

    FilaTurnosRPG turnos;
    FilaMensagensRPG mensagens;

    bool defendendo = false;
    float esperaMonstro = 0.0f;   // pausa antes do monstro agir
    float tempoFim = 0.0f;        // tempo mostrando o resultado
    ResultadoBatalhaRPG resultado = ResultadoBatalhaRPG::NENHUM;

    bool tecla1 = false;
    bool tecla2 = false;
    bool tecla3 = false;
    bool tecla4 = false;
};

// Apaga as duas filas.
void limparBatalhaRPG(BatalhaRPG& batalha) {
    while (batalha.turnos.inicio != nullptr) {
        desenfileirarTurnoRPG(batalha.turnos);
    }

    while (batalha.mensagens.inicio != nullptr) {
        NoMensagemRPG* apagar = batalha.mensagens.inicio;
        batalha.mensagens.inicio = apagar->proximo;
        delete apagar;
    }
    batalha.mensagens.fim = nullptr;
    batalha.mensagens.tamanho = 0;
}

// Estado unico da batalha (global, assim o main nao precisa declarar nada).
// O ponteiro e liberado automaticamente ao fechar o programa.
struct GuardaBatalhaRPG {
    BatalhaRPG dados;
    ~GuardaBatalhaRPG() { limparBatalhaRPG(dados); }
};
GuardaBatalhaRPG guardaBatalhaRPG;
BatalhaRPG& batalhaRPG = guardaBatalhaRPG.dados;

// ---------- Fonte (carregada na primeira batalha) ----------
Font fonteBatalhaRPG;
bool fonteCarregadaRPG = false;
bool fonteTentadaRPG = false;

// Tenta carregar uma fonte. Sem fonte a batalha funciona igual,
// mas os textos aparecem apenas no console.
void carregarFonteBatalhaRPG() {
    if (fonteTentadaRPG) {
        return;
    }
    fonteTentadaRPG = true;

    const char* caminhos[] = {
        "fonts/arial.ttf",
        "arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf"
    };

    for (const char* caminho : caminhos) {
        if (fonteBatalhaRPG.openFromFile(caminho)) {
            fonteCarregadaRPG = true;
            return;
        }
    }

    cout << "Aviso: nenhuma fonte encontrada. Coloque um arquivo "
         << "arial.ttf na pasta do jogo para ver os textos da batalha.\n";
}

// Escreve uma mensagem no log da batalha (e no console).
void mensagemBatalhaRPG(BatalhaRPG& batalha, const string& texto) {
    enfileirarMensagemRPG(batalha.mensagens, texto);
    cout << texto << '\n';
}

// Aplica o dano primeiro na armadura e depois na vida.
// Retorna quanto de vida o jogador realmente perdeu.
int aplicarDanoBatalhaRPG(JogadorRPG& jogador, int dano) {
    dano = max(0, dano);
    int danoArmadura = min(dano, jogador.resistenciaArmadura);
    int danoVida = dano - danoArmadura;

    jogador.resistenciaArmadura -= danoArmadura;
    jogador.vida = max(0, jogador.vida - danoVida);

    if (jogador.resistenciaArmadura == 0) {
        jogador.armaduraColetada = false;
        if (danoArmadura > 0) {
            cout << "\nSua armadura quebrou!\n";
        }
    }

    return danoVida;
}

// Comeca uma batalha contra o monstro da sala.
void iniciarBatalhaRPG(BatalhaRPG& batalha, JogadorRPG& jogador, Sala* sala) {
    carregarFonteBatalhaRPG();
    limparBatalhaRPG(batalha);

    batalha.ativa = true;
    batalha.monstro = sala->monstro;
    batalha.ehBoss = sala->ehBoss;
    batalha.vidaInicialMonstro = max(1, sala->monstro->vida);
    batalha.defendendo = false;
    batalha.esperaMonstro = 0.9f;
    batalha.tempoFim = 0.0f;
    batalha.resultado = ResultadoBatalhaRPG::NENHUM;

    jogador.inventarioAberto = false;

    // O jogador sempre comeca.
    enfileirarTurnoRPG(batalha.turnos, ATOR_JOGADOR);
    enfileirarTurnoRPG(batalha.turnos, ATOR_MONSTRO);

    cout << "\n========== BATALHA ==========\n";
    if (batalha.ehBoss) {
        mensagemBatalhaRPG(batalha, "O BOSS " + batalha.monstro->nome + " te desafia!");
    } else {
        mensagemBatalhaRPG(batalha, "Um " + batalha.monstro->nome + " apareceu!");
    }
}

// Passa a vez: quem agiu sai do inicio e volta para o fim da fila.
void passarTurnoRPG(BatalhaRPG& batalha) {
    int ator = desenfileirarTurnoRPG(batalha.turnos);
    enfileirarTurnoRPG(batalha.turnos, ator);
    batalha.esperaMonstro = 0.9f;
}

// Verifica se alguem morreu e encerra a batalha se for o caso.
void verificarFimBatalhaRPG(BatalhaRPG& batalha, const JogadorRPG& jogador) {
    if (batalha.monstro->vida <= 0) {
        batalha.resultado = ResultadoBatalhaRPG::VITORIA;
        batalha.tempoFim = 1.6f;
        mensagemBatalhaRPG(batalha, batalha.monstro->nome + " foi derrotado!");
    } else if (jogador.vida <= 0) {
        batalha.resultado = ResultadoBatalhaRPG::DERROTA;
        batalha.tempoFim = 1.6f;
        mensagemBatalhaRPG(batalha, "Voce foi derrotado...");
    }
}

// Turno do jogador: espera uma das teclas 1 a 4.
void turnoJogadorBatalhaRPG(
    BatalhaRPG& batalha,
    JogadorRPG& jogador,
    bool k1, bool k2, bool k3, bool k4,
    mt19937& gerador
) {
    bool agiu = false;

    if (k1) {
        // Atacar
        uniform_int_distribution<int> sorteio(0, 5);
        int dano = 15 + sorteio(gerador);

        batalha.monstro->vida = max(0, batalha.monstro->vida - dano);
        mensagemBatalhaRPG(batalha, "Voce atacou! Dano: " + to_string(dano));
        agiu = true;
    }
    else if (k2) {
        // Pocao (so gasta o turno se realmente curar)
        if (jogador.quantidadePocoes == 0) {
            mensagemBatalhaRPG(batalha, "Voce nao tem pocoes!");
        } else if (jogador.vida == jogador.vidaMax) {
            mensagemBatalhaRPG(batalha, "Sua vida ja esta completa!");
        } else {
            usarPocaoRPG(jogador);
            mensagemBatalhaRPG(batalha, "Voce usou uma pocao. Vida: " +
                to_string(jogador.vida) + "/" + to_string(jogador.vidaMax));
            agiu = true;
        }
    }
    else if (k3) {
        // Defender: reduz o proximo dano pela metade
        batalha.defendendo = true;
        mensagemBatalhaRPG(batalha, "Voce se defende e reduz o proximo dano.");
        agiu = true;
    }
    else if (k4) {
        // Fugir: 50% de chance, impossivel contra o Boss
        if (batalha.ehBoss) {
            mensagemBatalhaRPG(batalha, "Nao ha como fugir do Boss!");
            agiu = true;
        } else {
            uniform_int_distribution<int> sorteio(0, 99);

            if (sorteio(gerador) < 50) {
                mensagemBatalhaRPG(batalha, "Voce fugiu da batalha!");
                batalha.resultado = ResultadoBatalhaRPG::FUGA;
                batalha.tempoFim = 1.0f;
                return;
            }
            mensagemBatalhaRPG(batalha, "Voce nao conseguiu fugir!");
            agiu = true;
        }
    }

    if (!agiu) {
        return;
    }

    verificarFimBatalhaRPG(batalha, jogador);

    if (batalha.resultado == ResultadoBatalhaRPG::NENHUM) {
        passarTurnoRPG(batalha);
    }
}

// Turno do monstro: age sozinho depois de uma pequena pausa.
void turnoMonstroBatalhaRPG(
    BatalhaRPG& batalha,
    JogadorRPG& jogador,
    float tempo,
    mt19937& gerador
) {
    batalha.esperaMonstro -= tempo;

    if (batalha.esperaMonstro > 0) {
        return;
    }

    uniform_int_distribution<int> sorteio(0, 3);
    int dano = batalha.monstro->ataque + sorteio(gerador);

    // Itens recusados aumentam o dano do Boss.
    if (batalha.ehBoss) {
        dano += jogador.itensRecusados * 2;
    }

    if (batalha.defendendo) {
        dano = max(1, dano / 2);
        batalha.defendendo = false;
    }

    int danoVida = aplicarDanoBatalhaRPG(jogador, dano);

    mensagemBatalhaRPG(batalha, batalha.monstro->nome + " atacou! Dano: " +
        to_string(dano) + " (vida perdida: " + to_string(danoVida) + ")");

    verificarFimBatalhaRPG(batalha, jogador);

    if (batalha.resultado == ResultadoBatalhaRPG::NENHUM) {
        passarTurnoRPG(batalha);
    }
}

// Atualiza a batalha a cada quadro.
void atualizarBatalhaRPG(
    BatalhaRPG& batalha,
    JogadorRPG& jogador,
    float tempo,
    mt19937& gerador
) {
    // Sempre le as teclas para manter o controle de "acabou de apertar".
    bool k1 = pressionouTeclaRPG(sf::Keyboard::Key::Num1, batalha.tecla1);
    bool k2 = pressionouTeclaRPG(sf::Keyboard::Key::Num2, batalha.tecla2);
    bool k3 = pressionouTeclaRPG(sf::Keyboard::Key::Num3, batalha.tecla3);
    bool k4 = pressionouTeclaRPG(sf::Keyboard::Key::Num4, batalha.tecla4);

    // Batalha acabou: so espera o tempo de exibir o resultado.
    if (batalha.resultado != ResultadoBatalhaRPG::NENHUM) {
        batalha.tempoFim -= tempo;
        return;
    }

    if (turnoAtualRPG(batalha.turnos) == ATOR_JOGADOR) {
        turnoJogadorBatalhaRPG(batalha, jogador, k1, k2, k3, k4, gerador);
    } else {
        turnoMonstroBatalhaRPG(batalha, jogador, tempo, gerador);
    }
}

// Depois do tempo de resultado, fecha a batalha e devolve o resultado.
ResultadoBatalhaRPG encerrarBatalhaRPG(
    BatalhaRPG& batalha,
    JogadorRPG& jogador,
    Sala* sala
) {
    ResultadoBatalhaRPG resultado = batalha.resultado;

    batalha.ativa = false;
    limparBatalhaRPG(batalha);

    // Volta o jogador ao centro e da um tempo para nao reiniciar a batalha.
    jogador.corpo.setPosition({245.0f, 120.0f});
    jogador.tempoVulneravel = 1.5f;

    // Se fugiu, o monstro e afastado do jogador.
    if (resultado == ResultadoBatalhaRPG::FUGA &&
        sala->monstro != nullptr) {
        EstadoMonstroRPG* estado =
            obterEstadoMonstroRPG(sala->monstro, sala->ehBoss);
        estado->corpo.setPosition({420.0f, 50.0f});
    }

    return resultado;
}

// ---------- Desenho da tela de batalha ----------

void desenharTextoBatalhaRPG(
    sf::RenderWindow& janela,
    const string& conteudo,
    sf::Vector2f posicao,
    unsigned int tamanho,
    sf::Color cor
) {
    if (!fonteCarregadaRPG) {
        return;
    }

    sf::Text texto(fonteBatalhaRPG, conteudo, tamanho);
    texto.setPosition(posicao);
    texto.setFillColor(cor);
    janela.draw(texto);
}

void desenharBatalhaRPG(
    sf::RenderWindow& janela,
    const BatalhaRPG& batalha,
    const JogadorRPG& jogador
) {
    // Fundo
    sf::RectangleShape fundo({512.0f, 256.0f});
    fundo.setFillColor(sf::Color(28, 22, 38));
    janela.draw(fundo);

    // Chao da arena
    sf::RectangleShape arena({512.0f, 22.0f});
    arena.setPosition({0.0f, 128.0f});
    arena.setFillColor(sf::Color(55, 45, 70));
    janela.draw(arena);

    // ----- Monstro (direita) -----
    Monstro* m = batalha.monstro;
    float tam = batalha.ehBoss ? 80.0f : 56.0f;

    sf::RectangleShape corpoMonstro({tam, tam});
    corpoMonstro.setPosition({390.0f - tam / 2, 128.0f - tam});
    corpoMonstro.setFillColor(batalha.ehBoss ? sf::Color::Red : sf::Color(150, 50, 50));
    if (m->vida <= 0) {
        corpoMonstro.setFillColor(sf::Color(80, 80, 80));
    }
    janela.draw(corpoMonstro);

    desenharTextoBatalhaRPG(janela, (batalha.ehBoss ? "BOSS " : "") + m->nome,
        {290.0f, 6.0f}, 13, sf::Color::White);

    desenharBarraRPG(janela, {290.0f, 24.0f}, 190.0f, 9.0f,
        m->vida, batalha.vidaInicialMonstro, sf::Color(220, 50, 50));
    desenharTextoBatalhaRPG(janela,
        "HP " + to_string(m->vida) + "/" + to_string(batalha.vidaInicialMonstro),
        {290.0f, 35.0f}, 10, sf::Color(220, 220, 220));

    // ----- Jogador (esquerda) -----
    sf::RectangleShape corpoJogador({36.0f, 48.0f});
    corpoJogador.setPosition({100.0f, 80.0f});
    corpoJogador.setFillColor(sf::Color::Blue);
    if (batalha.defendendo) {
        corpoJogador.setOutlineThickness(3.0f);
        corpoJogador.setOutlineColor(sf::Color::Cyan);
    }
    janela.draw(corpoJogador);

    desenharTextoBatalhaRPG(janela, "Voce", {30.0f, 6.0f}, 13, sf::Color::White);

    desenharBarraRPG(janela, {30.0f, 24.0f}, 190.0f, 9.0f,
        jogador.vida, jogador.vidaMax, sf::Color::Green);
    desenharTextoBatalhaRPG(janela,
        "HP " + to_string(jogador.vida) + "/" + to_string(jogador.vidaMax),
        {30.0f, 35.0f}, 10, sf::Color(220, 220, 220));

    desenharBarraRPG(janela, {30.0f, 50.0f}, 190.0f, 6.0f,
        jogador.resistenciaArmadura, jogador.resistenciaMax, sf::Color::Cyan);
    desenharTextoBatalhaRPG(janela,
        "ARM " + to_string(jogador.resistenciaArmadura) + "/" + to_string(jogador.resistenciaMax),
        {30.0f, 57.0f}, 10, sf::Color(220, 220, 220));

    // ----- Painel inferior -----
    sf::RectangleShape painel({504.0f, 100.0f});
    painel.setPosition({4.0f, 152.0f});
    painel.setFillColor(sf::Color(15, 12, 22));
    painel.setOutlineThickness(2.0f);
    painel.setOutlineColor(sf::Color(160, 160, 190));
    janela.draw(painel);

    // Indicador de turno
    bool vezDoJogador =
        batalha.resultado == ResultadoBatalhaRPG::NENHUM &&
        turnoAtualRPG(batalha.turnos) == ATOR_JOGADOR;

    string indicador;
    if (batalha.resultado == ResultadoBatalhaRPG::VITORIA) indicador = "VITORIA!";
    else if (batalha.resultado == ResultadoBatalhaRPG::DERROTA) indicador = "DERROTA...";
    else if (batalha.resultado == ResultadoBatalhaRPG::FUGA) indicador = "FUGA!";
    else if (vezDoJogador) indicador = "Seu turno";
    else indicador = "Turno do inimigo...";

    desenharTextoBatalhaRPG(janela, indicador, {400.0f, 156.0f}, 12,
        vezDoJogador ? sf::Color::Yellow : sf::Color(200, 200, 200));

    // Log de mensagens (percorre a fila do inicio ao fim)
    float y = 158.0f;
    NoMensagemRPG* msg = batalha.mensagens.inicio;
    while (msg != nullptr) {
        desenharTextoBatalhaRPG(janela, msg->texto, {12.0f, y}, 11, sf::Color::White);
        y += 15.0f;
        msg = msg->proximo;
    }

    // Botoes de acao
    string rotulos[4] = {
        "1 Atacar",
        "2 Pocao (" + to_string(jogador.quantidadePocoes) + ")",
        "3 Defender",
        "4 Fugir"
    };
    sf::Color cores[4] = {
        sf::Color(170, 60, 60),
        sf::Color(60, 150, 70),
        sf::Color(60, 100, 170),
        sf::Color(150, 130, 50)
    };

    for (int i = 0; i < 4; i++) {
        sf::RectangleShape botao({116.0f, 24.0f});
        botao.setPosition({10.0f + i * 124.0f, 222.0f});

        sf::Color cor = cores[i];
        if (!vezDoJogador) {
            cor = sf::Color(cor.r / 3, cor.g / 3, cor.b / 3);
        }
        botao.setFillColor(cor);
        botao.setOutlineThickness(1.0f);
        botao.setOutlineColor(sf::Color(220, 220, 220));
        janela.draw(botao);

        desenharTextoBatalhaRPG(janela, rotulos[i],
            {18.0f + i * 124.0f, 226.0f}, 12, sf::Color::White);
    }
}

// ============================================================================
// FUNCOES CHAMADAS PELO MAIN (uma linha cada)
// ============================================================================

// GANCHO 1 - chamar logo depois de calcular o dt.
// Se a batalha esta ativa, atualiza, desenha e retorna true (o main faz continue).
// Se acabou, devolve false e o jogo segue normal (derrota cai no GAME OVER
// que ja existe no main). Vencer o Boss fecha a janela.
bool processarBatalhaRPG(
    JogadorRPG& jogador,
    Sala* sala,
    float dt,
    mt19937& gerador,
    sf::RenderWindow& janela
) {
    BatalhaRPG& b = batalhaRPG;

    if (!b.ativa) {
        return false;
    }

    atualizarBatalhaRPG(b, jogador, dt, gerador);

    if (b.resultado != ResultadoBatalhaRPG::NENHUM && b.tempoFim <= 0) {
        ResultadoBatalhaRPG resultado = encerrarBatalhaRPG(b, jogador, sala);

        if (resultado == ResultadoBatalhaRPG::VITORIA && sala->ehBoss) {
            cout << "\n====================" << endl;
            cout << "      VITORIA!      " << endl;
            cout << "Voce derrotou o Boss!" << endl;
            cout << "====================" << endl;
            janela.close();
            return true;
        }
        return false;
    }

    desenharBatalhaRPG(janela, b, jogador);
    janela.display();
    return true;
}

// GANCHO 2 - chamar ANTES de atualizarCombateSalaRPG.
// Se o jogador encostou no monstro, inicia a batalha e retorna true
// (o main faz continue, evitando o dano por contato antigo).
bool verificarEncontroRPG(JogadorRPG& jogador, Sala* sala) {
    if (sala == nullptr ||
        sala->monstro == nullptr ||
        sala->monstro->vida <= 0) {
        return false;
    }

    // Folga logo depois de uma batalha
    if (jogador.tempoVulneravel > 0) {
        return false;
    }

    EstadoMonstroRPG* estado =
        obterEstadoMonstroRPG(sala->monstro, sala->ehBoss);

    if (detectarColisaoAABBRPG(jogador.corpo, estado->corpo)) {
        iniciarBatalhaRPG(batalhaRPG, jogador, sala);
        return true;
    }

    return false;
}
