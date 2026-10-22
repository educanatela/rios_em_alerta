
const ENDERECO_ESP32 = "http://192.168.4.1/dados";
const PROFUNDIDADE_RIO = 100;

// Elementos do painel
const distanciaElemento = document.getElementById("distancia");
const nivelElemento = document.getElementById("nivel");
const statusElemento = document.getElementById("status");
const mensagemElemento = document.getElementById("mensagemStatus");
const horaElemento = document.getElementById("hora");
const porcentagemElemento = document.getElementById("porcentagem");
const barraElemento = document.getElementById("barraProgresso");
const statusCard = document.getElementById("statusCard");
const statusIcone = document.getElementById("statusIcone");
const indicadorConexao = document.getElementById("indicadorConexao");
const textoConexao = document.getElementById("textoConexao");

// Atualiza a conexão
function definirConexao(conectado) {
    indicadorConexao.style.background = conectado ? "#43a85c" : "#d84b4b";
    textoConexao.textContent = conectado ? "Sistema conectado" : "Sem conexão";
}

// Atualiza o painel
function atualizarPainel(distancia) {
    distanciaElemento.textContent = distancia.toFixed(1);

    let nivel = PROFUNDIDADE_RIO - distancia;
    nivelElemento.textContent = nivel;
    barraElemento.style.width = porcentagemElemento.textContent = nivel + "%";

    let status, mensagem, classe, cor;

    if (distancia > 50) {
        status = "NORMAL";
        mensagem = "Nível baixo. Sistema em condição normal.";
        classe = "status-normal";
        cor = "#43a85c";
    } else if (distancia > 25) {
        status = "ATENÇÃO";
        mensagem = "Nível subindo. Acompanhe a situação.";
        classe = "status-atencao";
        cor = "#d6a01d";
    } else {
        status = "ALERTA";
        mensagem = "Nível elevado. Situação de alerta.";
        classe = "status-alerta";
        cor = "#d84b4b";
    }

    statusCard.classList.remove(
        "status-normal", "status-atencao", "status-alerta"
    );

    statusCard.classList.add(classe);

    statusElemento.textContent = status;
    mensagemElemento.textContent = mensagem;
    statusIcone.textContent = "●";
    barraElemento.style.background = cor;

    horaElemento.textContent = new Date().toLocaleTimeString("pt-BR");
}

// Consulta a ESP32
async function atualizar() {
    try {
        const resposta = await fetch(ENDERECO_ESP32, { cache: "no-store" });
        if (!resposta.ok) throw new Error("Erro HTTP: " + resposta.status);

        const dados = await resposta.json();
        definirConexao(true);

        if (dados.distancia < 0 || dados.nivel === "Sem leitura") {
            distanciaElemento.textContent = "--";
            nivelElemento.textContent = "--";
            porcentagemElemento.textContent = "0%";
            barraElemento.style.width = "0%";
            barraElemento.style.background = "#8c9aa3";

            statusCard.classList.remove(
                "status-normal", "status-atencao", "status-alerta"
            );
            statusElemento.textContent = "SEM LEITURA";
            mensagemElemento.textContent = "Não foi possível obter uma leitura válida.";
            statusIcone.textContent = "●";
            horaElemento.textContent = new Date().toLocaleTimeString("pt-BR");
        } else {
            atualizarPainel(dados.distancia);
        }
    } catch (erro) {
        definirConexao(false);
        console.error("Erro ao consultar a ESP32:", erro);
    }
}

// Atualização automática a cada segundo
atualizar();
setInterval(atualizar, 1000);