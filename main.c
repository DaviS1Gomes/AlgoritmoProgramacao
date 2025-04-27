#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ROWS 100
#define COLS 100

typedef struct
{
  int row;
  int col;
} Position;

typedef struct
{
  Position positions[ROWS * COLS];
  int front;
  int rear;
} Queue;

void initQueue(Queue *q)
{
  q->front = 0;
  q->rear = -1;
}

int isEmpty(Queue *q)
{
  return q->rear < q->front;
}

void enqueue(Queue *q, Position pos)
{
  q->positions[++q->rear] = pos;
}

Position dequeue(Queue *q)
{
  return q->positions[q->front++];
}

void readGridFromFile(char *matrix, int rows, int cols, const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    printf("Erro ao abrir o arquivo");
    return;
  }

  static char line[1024];
  int r = 0;

  while (fgets(line, sizeof(line), file) && r < rows)
  {
    int c = 0;
    char *pos = line;
    while (*pos != '\n' && c < cols)
    {
      *(matrix + r * cols + c) = *pos++;
      c++;
    }
    r++;
  }

  fclose(file);
}

void writeGridToFile(char *matrix, int rows, int cols, const char *filename)
{
  FILE *file = fopen(filename, "w");
  if (file == NULL)
  {
    perror("Erro ao abrir o arquivo para escrita");
    return;
  }

  for (int r = 0; r < rows; r++)
  {
    for (int c = 0; c < cols; c++)
    {
      fputc(*(matrix + r * cols + c), file);
    }
    fputc('\n', file);
  }

  fclose(file);
}

Position findPosition(char *matrix, char target)
{
  Position pos = {-1, -1};
  for (int r = 0; r < ROWS; r++)
  {
    for (int c = 0; c < COLS; c++)
    {
      if (*(matrix + r * COLS + c) == target)
      {
        pos.row = r;
        pos.col = c;
        return pos;
      }
    }
  }
  return pos;
}

void generateRandomGrid(char *grid, int obstaclePercentage)
{
  for (int r = 0; r < ROWS; r++)
  {
    for (int c = 0; c < COLS; c++)
    {
      *(grid + r * COLS + c) = ' ';
    }
  }

  Position a_pos, b_pos;
  do
  {
    a_pos.row = rand() % ROWS;
    a_pos.col = rand() % COLS;
    b_pos.row = rand() % ROWS;
    b_pos.col = rand() % COLS;
  } while ((a_pos.row == b_pos.row && a_pos.col == b_pos.col) ||
           (abs(a_pos.row - b_pos.row) + abs(a_pos.col - b_pos.col) < 50));

  *(grid + a_pos.row * COLS + a_pos.col) = 'A';
  *(grid + b_pos.row * COLS + b_pos.col) = 'B';

  int totalCells = ROWS * COLS - 2;
  int obstacleCount = (totalCells * obstaclePercentage) / 100;

  while (obstacleCount > 0)
  {
    int r = rand() % ROWS;
    int c = rand() % COLS;

    if (*(grid + r * COLS + c) == ' ')
    {
      *(grid + r * COLS + c) = 'x';
      obstacleCount--;
    }
  }
}

int isValid(char *grid, int row, int col, int visited[ROWS][COLS])
{
  if (row < 0 || col < 0 || row >= ROWS || col >= COLS)
    return 0;

  char cell = *(grid + row * COLS + col);
  return (cell == ' ' || cell == 'B') && !visited[row][col];
}

int findPath(char *grid, Position start, Position end, Position parent[ROWS][COLS])
{
  int visited[ROWS][COLS] = {0};
  int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

  Queue q;
  initQueue(&q);
  enqueue(&q, start);
  visited[start.row][start.col] = 1;

  while (!isEmpty(&q))
  {
    Position current = dequeue(&q);

    if (current.row == end.row && current.col == end.col)
    {
      return 1;
    }

    for (int i = 0; i < 4; i++)
    {
      int newRow = current.row + directions[i][0];
      int newCol = current.col + directions[i][1];

      if (isValid(grid, newRow, newCol, visited))
      {
        visited[newRow][newCol] = 1;
        parent[newRow][newCol] = current;
        Position next = {newRow, newCol};
        enqueue(&q, next);
      }
    }
  }

  return 0;
}

void markPathAndVisited(char *grid, Position start, Position end, Position parent[ROWS][COLS], int visited[ROWS][COLS])
{
  for (int r = 0; r < ROWS; r++)
  {
    for (int c = 0; c < COLS; c++)
    {
      if (visited[r][c] && *(grid + r * COLS + c) == ' ')
      {
        *(grid + r * COLS + c) = 'o';
      }
    }
  }

  Position current = end;
  while (!(current.row == start.row && current.col == start.col))
  {
    if (*(grid + current.row * COLS + current.col) == ' ')
    {
      *(grid + current.row * COLS + current.col) = '.';
    }
    current = parent[current.row][current.col];
  }
}

void displayMenu()
{
  printf("\n=== MENU ===\n");
  printf("1. Gerar um Grid Aleatorio\n");
  printf("2. Resolver Caminho\n");
  printf("3. Sair\n");
  printf("Escolha uma opcao: ");
}

int main()
{
  srand(time(NULL));

  char grid[ROWS][COLS];
  int option;

  do
  {
    displayMenu();
    scanf("%d", &option);

    switch (option)
    {
    case 1:
    {
      int percentage;
      printf("Digite a porcentagem de obstaculos (0-100): ");
      scanf("%d", &percentage);

      if (percentage < 0 || percentage > 100)
      {
        printf("Porcentagem invalida!\n");
        break;
      }

      generateRandomGrid(&grid[0][0], percentage);
      writeGridToFile(&grid[0][0], ROWS, COLS, "grid.txt");
      printf("Grid gerado e salvo em grid.txt\n");
      break;
    }

    case 2:
    {
      readGridFromFile(&grid[0][0], ROWS, COLS, "grid.txt");

      Position start = findPosition(&grid[0][0], 'A');
      Position end = findPosition(&grid[0][0], 'B');

      if (start.row == -1 || start.col == -1)
      {
        printf("Ponto de partida 'A' nao encontrado no grid!\n");
        break;
      }

      if (end.row == -1 || end.col == -1)
      {
        printf("Ponto de chegada 'B' nao encontrado no grid!\n");
        break;
      }

      Position parent[ROWS][COLS];
      for (int r = 0; r < ROWS; r++)
      {
        for (int c = 0; c < COLS; c++)
        {
          parent[r][c].row = -1;
          parent[r][c].col = -1;
        }
      }

      if (findPath(&grid[0][0], start, end, parent))
      {
        printf("Caminho encontrado!\n");

        int visited[ROWS][COLS] = {0};
        Queue q;
        initQueue(&q);
        enqueue(&q, start);
        visited[start.row][start.col] = 1;

        int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        while (!isEmpty(&q))
        {
          Position current = dequeue(&q);

          for (int i = 0; i < 4; i++)
          {
            int newRow = current.row + directions[i][0];
            int newCol = current.col + directions[i][1];

            if (isValid(&grid[0][0], newRow, newCol, visited))
            {
              visited[newRow][newCol] = 1;
              Position next = {newRow, newCol};
              enqueue(&q, next);
            }
          }
        }

        markPathAndVisited(&grid[0][0], start, end, parent, visited);
        writeGridToFile(&grid[0][0], ROWS, COLS, "path.txt");
        printf("Grid com caminho salvo em path.txt\n");
      }
      else
      {
        printf("Nao foi possivel encontrar um caminho de A ate B!\n");
      }
      break;
    }

    case 3:
      printf("Saindo...\n");
      break;

    default:
      printf("Opcao invalida!\n");
    }
  } while (option != 3);

  return 0;
}