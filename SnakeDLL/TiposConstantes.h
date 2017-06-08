#pragma once

/* ----------------------------------------------------- */
/*  CONSTANTES											 */
/* ----------------------------------------------------- */
#define SIZE_USERNAME		30									// Max chars do utilizador
#define TAM_BUFFER			25									// Tamanho de Buffer a utilizar no CLiente
#define MAXCLIENTES			4									// Maximo de Clientes 
#define MAXJOGADORES		4									// Max jogadores permitido
#define MAXOBJECTOS			30									// Max objectos no mapa permitidos
#define MAX_LINHAS			40									// Limite maximo de Linhas
#define MAX_COLUNAS			80									// Limite maximo de Colunas
#define MIN_LINHAS			10									// Limite minimo de Linhas
#define MIN_COLUNAS			10									// Limite minimo de Linhas
#define MAX_PEDIDOS			5									// Limite maximo na Fila de Pedidos
#define NUMTIPOOBJECTOS		10									// Tipo de objectos existentes
#define MAXSEGUNDOSMAPA		30									// Maximo de segundos que objectos ficam no mapa
#define MINSEGUNDOSMAPA		5									// Minimo de segundos que objectos ficam no mapa
#define SIZEMENSAGEM		sizeof(Msg)							// Tamanho da estrutura Msg
#define SIZE_MEM_GERAL		sizeof(MemGeral)					// Tamanho da Memoria Partilhada Geral
#define NOME_MEM_GERAL		TEXT("SharedMemGeral")				// Nome da Memoria Partilhada Geral
#define NOME_MEM_RESPOSTA	TEXT("SharedMemResp_%d")				// Nome da Memoria de Resposta para um cliente especifico
#define NOME_SEM_MAPA		TEXT("SemaforoMapa")				// Nome do Semaforo do Mapa
#define NOME_EVNT_MAPA		TEXT("EventoMapa")					// Nome do Evento do Mapa
#define NOME_EVNT_RESPOSTA	TEXT("EventoResposta_%d")				// Nome do Evento de Respostas para cada cliente que se Liga, será concatenado com o Pid
#define NOME_SEM_PODELER	TEXT("SemaforoPedidosPodeLer")		// Nome do Semaforo dos Pedidos para avisar que há pedidos para ler
#define NOME_SEM_PODESCRVR	TEXT("SemaforoPedidosPodeEscrever")	// Nome do Semaforo dos Pedidos para avisar que há espaço para escrever
#define NOME_FILE_MAP		TEXT("backup.txt")					// Nome do Ficheiro mapeado em memoria

//Estados de Jogo
#define CRIACAOJOGO			1
#define ASSOCIACAOJOGO		2
#define DECORRERJOGO		3
#define FINALJOGO			4

//Estados do Jogador
#define VIVO				1
#define MORTO				2
#define BEBADO				3
#define LEBRE				4
#define TARTARUGA			5

//Tipos de Servidor
#define LOCAL				1
#define REMOTO				2

//Movimentos das Serpentes (usado na fila de pedidos e nos pipes)
#define CIMA				1
#define BAIXO				2
#define ESQUERDA			3
#define DIREITA				4

//Tipos de Mensagem (usado na fila de pedidos e nos pipes)
#define CRIARJOGO			5
#define ASSOCIAR_JOGADOR1	6
#define ASSOCIAR_JOGADOR2	7
#define INICIARJOGO			8
#define REGISTACLIENTELOCAL	9
#define REGISTACLIENTEREMTO 10

//Identificador de Jogador para saber se é o jogador 1 ou 2 de determinada maquina
#define JOGADOR1			1
#define JOGADOR2			2

/*************************ELEMENTOS DO MAPA*************************/
#define PAREDE				-1
#define ESPACOVAZIO			0
//Objectos
#define ALIMENTO			1 
#define GELO				2
#define GRANADA				3 
#define VODKA				4 
#define OLEO				5 
#define COLA				6 
#define O_VODKA				7 
#define O_OLEO				8 
#define O_COLA				9 
#define SURPRESA			10  

//Valores configuraveis por defeito
#define LINHAS				40
#define COLUNAS				40
#define TAMANHOSNAKE		3
#define NUMJOGADORES		1
#define NUMAUTOSNAKE		1
#define NUMOBJETOS			6
#define SEGUNDOSMAPA		10
#define PERMANENTE			-1

//Factores de Lentidão
#define LENTIDAO_NORMAL		750 //valor em milisegundos da lentidão das cobras
#define LENTIDAO_LEBRE		500
#define LENTIDAO_TARTARUGA	1000
#define SEGUNDO				1000 //valor em milisegundos de um segundo para ser usado nos objectos

//ciclos de duração dos efeitos nas cobras
#define CICLOS_TARTARUGA	10 //10 ciclos = 10 segundos de duração do efeito
#define CICLOS_LEBRE		20 //20 ciclos = 10 segundos de duração do efeito
#define CICLOS_VODKA		13 //13 ciclos = 9 segundos e 750 milisegundos de duração do efeito :(

//CONSTANTES DE PROBABILIDADES
#define PROB_ALIMENTO		500
#define PROB_GELO			700
#define PROB_OLEO			900
#define PROB_COLA			1100
#define PROB_VODKA			1150
#define PROB_O_VODKA		1200
#define PROB_O_OLEO			1250
#define PROB_O_COLA			1300
#define PROB_SURPRESA		1350
#define PROB_GRANADA		1400

//CONSTANTES PARA AS RESPOSTAS AOS CLIENTES
#define	SUCESSO				1
#define	INSUCESSO			2
#define	AGORANAO			-1
#define JOGOCHEIO			-2
#define CRIADORERRADO		-3

/* ----------------------------------------------------- */
/*  TIPOS												 */
/* ----------------------------------------------------- */
typedef struct {
	int PId;
	int codigoMsg;
}Mensagem;

typedef struct {
	int T;			//Tamanho inicial das Serpentes
	int A;			//Numero de Serpentes Automáticas
	int O;			//Numero de Objectos
	int N;			//Numero maximo de jogadores
	int L;			//Tamanho do Mapa em Linhas
	int C;			//Tamanho do Mapa em Colunas
}ConfigInicial;

//Estrutura para manutenção dos objectos no jogo
typedef struct {
	int Tipo;				//Tipo de Objecto (1-Alimento, 2-Gelo, 3-Granada, 4-Vodka, 5-Oleo, 6-Cola, 7-OVodka, 8-OOleo, 9-OCola)
	int linha;				//Posição no mapa
	int coluna;				//Posição no mapa
	int segundosRestantes;	//Segundos que restam ao objecto para este desaparecer
}Objecto;

//Estrutura de configuração dos objectos
typedef struct {
	int Tipo;		//Tipo de Objecto (1-Alimento, 2-Gelo, 3-Granada, 4-Vodka, 5-Oleo, 6-Cola, 7-OVodka, 8-OOleo, 9-OCola)
	int S;			//Segundos que fica no mapa
}ConfigObjecto;

typedef struct {
	int pid;
	int codigoPedido;
	int jogador;
	ConfigInicial config;
	TCHAR username[SIZE_USERNAME];
	ConfigObjecto objectosConfig[NUMTIPOOBJECTOS];
}Pedido;

typedef struct {
	int resposta;
	int valor;
}Resposta;

typedef struct {
	int pid;
	int remoto;
	HANDLE hEventoResposta;
	HANDLE hMemResposta;
	Resposta *vistaResposta;
	HANDLE hPipe;
}Cliente;

typedef struct {
	Pedido pedidos[MAX_PEDIDOS];
	int frente;
	int tras;
}Fila_Pedidos;

typedef struct {
	Fila_Pedidos fila;
	int mapa[MAX_LINHAS][MAX_COLUNAS];
	int colunas;
	int linhas;
}MemGeral;