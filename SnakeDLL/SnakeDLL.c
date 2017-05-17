
#include <windows.h>
#include "SnakeDLL.h"

//Definição da variável global
HANDLE hMemoria;
HANDLE hSemMemoria;
HANDLE hEventoMemoria;
HANDLE hFicheiro;
MemGeral *vistaPartilhaGeral;

void criaCobra(TCHAR username[SIZE_USERNAME], int vaga);

int preparaMemoriaPartilhada(void) {

	hFicheiro = CreateFile(FILE_MAP_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	hMemoria = CreateFileMapping(hFicheiro, NULL, PAGE_READWRITE, 0, SIZE_MEM_GERAL, NOME_MEM_GERAL);

	vistaPartilhaGeral = (MemGeral*)MapViewOfFile(hMemoria, FILE_MAP_ALL_ACCESS, 0, 0, SIZE_MEM_GERAL);

	hEventoMemoria = CreateEvent(NULL, TRUE, FALSE, EVNT_MEM_GERAL);
	hSemMemoria = CreateSemaphore(NULL, MAXCLIENTES, MAXCLIENTES, SEM_MEM_GERAL);

	srand((int)time(NULL));

	if (hMemoria == NULL || hEventoMemoria == NULL || hSemMemoria == NULL) {
		_tprintf(TEXT("[Erro] Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}
}

void preparaMapaJogo(MemGeral param) {

	for (int i = 0; i < MAXCLIENTES; i++) {
		WaitForSingleObject(hSemMemoria, INFINITE);
	}

	for (int z = 0; z < param.config.C; z++) {
		vistaPartilhaGeral->mapa[0][z] = TEXT('#');
		vistaPartilhaGeral->mapa[param.config.L - 1][z] = TEXT('#');
		for (int j = 1; j < param.config.L - 1; j++) {
			if (z == 0 || z == param.config.C - 1) {
				vistaPartilhaGeral->mapa[j][z] = TEXT('#');
			}
			else
				vistaPartilhaGeral->mapa[j][z] = TEXT(' ');
		}
	}
	SetEvent(hEventoMemoria);
	ResetEvent(hEventoMemoria);
	ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
}

void inicializaMemoriaPartilhada(void) {
	//Inicialização da Memoria Partilhada
	vistaPartilhaGeral->estadoJogo = CRIACAOJOGO;
}

void esperaPorActualizacao(void) {
	WaitForSingleObject(hEventoMemoria, INFINITE);
}

void leMemoriaPartilhada(MemGeral* param) {

	WaitForSingleObject(hSemMemoria, INFINITE);

	param->estadoJogo = vistaPartilhaGeral->estadoJogo;
	param->mensagem.codigoMsg = vistaPartilhaGeral->mensagem.codigoMsg;
	_tcscpy_s(param->mensagem.username, SIZE_USERNAME, vistaPartilhaGeral->mensagem.username);
	param->config.C = vistaPartilhaGeral->config.C;
	param->config.L = vistaPartilhaGeral->config.L;

	ReleaseSemaphore(hSemMemoria, 1, NULL);
}

void fechaMemoriaPartilhada(void) {
	CloseHandle(hMemoria);
	CloseHandle(hSemMemoria);
	CloseHandle(hEventoMemoria);
	UnmapViewOfFile(vistaPartilhaGeral);
}

void getMapa(MemGeral *param) {
	WaitForSingleObject(hSemMemoria, INFINITE);
	for (int i = 0; i < vistaPartilhaGeral->config.L; i++) {
		for (int j = 0; j < vistaPartilhaGeral->config.C; j++) {
			param->mapa[i][j] = vistaPartilhaGeral->mapa[i][j];
		}
	}
	ReleaseSemaphore(hSemMemoria, 1, NULL);
}

int Cria_Jogo(ConfigInicial param,TCHAR username1[SIZE_USERNAME]) {
	for (int i = 0; i < MAXCLIENTES; i++) {
		WaitForSingleObject(hSemMemoria, INFINITE);
	}
	if ((vistaPartilhaGeral->estadoJogo != CRIACAOJOGO)) {
		ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
		return 0;
	}
	
	vistaPartilhaGeral->estadoJogo = ASSOCIACAOJOGO;
	vistaPartilhaGeral->config = param;
	vistaPartilhaGeral->mensagem.codigoMsg = CRIARJOGO;
	_tcscpy_s(vistaPartilhaGeral->mensagem.username, SIZE_USERNAME, username1);
	_tcscpy_s(vistaPartilhaGeral->criador, SIZE_USERNAME, username1);
	vistaPartilhaGeral->vagasJogadores = 0;

	SetEvent(hEventoMemoria);
	ResetEvent(hEventoMemoria);
	ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
	return 1;
}

int IniciaJogo(TCHAR username[SIZE_USERNAME]) {
	for (int i = 0; i < MAXCLIENTES; i++) {
		WaitForSingleObject(hSemMemoria, INFINITE);
	}

	//Se não for o user que criou o jogo não pode dar inicio a este
	if (!(_tcscmp(username, vistaPartilhaGeral->criador)==0)) {
		ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
		return 0;
	}
	vistaPartilhaGeral->mensagem.codigoMsg = INICIARJOGO;
	_tcscpy_s(vistaPartilhaGeral->mensagem.username, SIZE_USERNAME, username);

	SetEvent(hEventoMemoria);
	ResetEvent(hEventoMemoria);
	ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
	return 1;
}


int AssociaJogo(int numJogadores, TCHAR username1[SIZE_USERNAME], TCHAR username2[SIZE_USERNAME], int *indice) {
	for (int i = 0; i < MAXCLIENTES; i++) {
		WaitForSingleObject(hSemMemoria, INFINITE);
	}
	//Se não existir jogo criado e as vagas existentes não forem suficientes para os jogadores que se pretendem juntar ao jogo
	if ((vistaPartilhaGeral->estadoJogo != ASSOCIACAOJOGO) && ((vistaPartilhaGeral->config.N - vistaPartilhaGeral->vagasJogadores) < numJogadores)) {
		ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
		return 0;
	}
	*indice = vistaPartilhaGeral->vagasJogadores;

	if (numJogadores == 1) {
		criaCobra(username1, vistaPartilhaGeral->vagasJogadores);
		vistaPartilhaGeral->vagasJogadores++;
	}
	else {
		criaCobra(username1, vistaPartilhaGeral->vagasJogadores);
		vistaPartilhaGeral->vagasJogadores++;
		criaCobra(username2, vistaPartilhaGeral->vagasJogadores);
		vistaPartilhaGeral->vagasJogadores++;
	}

	SetEvent(hEventoMemoria);
	ResetEvent(hEventoMemoria);
	ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
	return 1;
}

void mudaDirecao(int direcao, int indice) {
	for (int i = 0; i < MAXCLIENTES; i++) {
		WaitForSingleObject(hSemMemoria, INFINITE);
	}

	//Hardcoded a posicao do array mas tem de ser mudado este aspecto
	vistaPartilhaGeral->jogadores[indice].direcao = direcao;

	ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
	return 1;
}

//Gera as posições da cobra no mapa verificando se há colisões com paredes e fazendo a respectiva alteração á cobra
void criaCobra(TCHAR username[SIZE_USERNAME], int vaga) {
	int posXGerada, posYGerada, dirGerada;
	//Gera posições até encontrar uma vaga;
	while (1) {
		posXGerada = random_at_most((long)vistaPartilhaGeral->config.C);
		posYGerada = random_at_most((long)vistaPartilhaGeral->config.L);
		if (vistaPartilhaGeral->mapa[posYGerada][posXGerada] == ' ')
			break;
	}

	//Na posição 0 do array de posições ficam as Linhas e na 1 ficam as Colunas
	vistaPartilhaGeral->jogadores[vaga].posicoesCobra[0][0] = posYGerada;
	vistaPartilhaGeral->jogadores[vaga].posicoesCobra[0][1] = posXGerada;
	vistaPartilhaGeral->mapa[posYGerada][posXGerada] = vaga + '0';

	dirGerada = random_at_most(3) + 1;
	vistaPartilhaGeral->jogadores[vaga].direcao = dirGerada;

	vistaPartilhaGeral->jogadores[vaga].porAparecer = vistaPartilhaGeral->config.T - 1;
	vistaPartilhaGeral->jogadores[vaga].estadoJogador = VIVO;
	vistaPartilhaGeral->jogadores[vaga].pontuacao = 0;
	vistaPartilhaGeral->jogadores[vaga].tamanho = vistaPartilhaGeral->config.T;
	_tcscpy_s(vistaPartilhaGeral->jogadores[vaga].username, SIZE_USERNAME, username);
}

// Assumes 0 <= max <= RAND_MAX
// Returns in the closed interval [0, max]
long random_at_most(long max) {
	unsigned long
		// max <= RAND_MAX < ULONG_MAX, so this is okay.
		num_bins = (unsigned long)max + 1,
		num_rand = (unsigned long)RAND_MAX + 1,
		bin_size = num_rand / num_bins,
		defect = num_rand % num_bins;

	long x;
	do {
		x = rand();
	}
	// This is carefully written not to overflow
	while (num_rand - defect <= (unsigned long)x);

	// Truncated division is intentional
	return x / bin_size;
}