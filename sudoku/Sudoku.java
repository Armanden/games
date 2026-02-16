import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Random;

public class Sudoku extends JFrame {
    public Sudoku() {
        setTitle("Sudoku");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(false);
        add(new SudokuPanel());
        pack();
        setLocationRelativeTo(null);
        setVisible(true);
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new Sudoku());
    }
}

class SudokuPanel extends JPanel {
    static final int GRID_SIZE = 9;
    static final int CELL_SIZE = 60;
    static final int PANEL_WIDTH = 540;
    static final int PANEL_HEIGHT = 600;

    Cell[][] grid = new Cell[9][9];
    int selectedRow = -1;
    int selectedCol = -1;
    int isWon = 0;
    int mistakes = 0;
    Random rand = new Random();

    static class Cell {
        int value;      // Current value (0 = empty)
        int original;   // Original puzzle value (can't be changed)

        Cell() {
            value = 0;
            original = 0;
        }
    }

    SudokuPanel() {
        setPreferredSize(new Dimension(PANEL_WIDTH, PANEL_HEIGHT));
        setBackground(Color.WHITE);
        setFocusable(true);

        // Initialize grid
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                grid[i][j] = new Cell();
            }
        }

        initializeGame();

        // Mouse listener
        addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                if (e.getY() < 540) {
                    int col = e.getX() / CELL_SIZE;
                    int row = e.getY() / CELL_SIZE;

                    if (row >= 0 && row < 9 && col >= 0 && col < 9) {
                        selectedRow = row;
                        selectedCol = col;
                        System.out.println("Selected: row=" + row + ", col=" + col);
                    }
                }
            }
        });

        // Key listener
        addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                handleInput(e);
            }
        });

        // Timer for repainting
        Timer timer = new Timer(16, e -> repaint());
        timer.start();
    }

    void handleInput(KeyEvent e) {
        int keyCode = e.getKeyCode();

        // Number input (1-9)
        if (keyCode >= KeyEvent.VK_1 && keyCode <= KeyEvent.VK_9) {
            int num = keyCode - KeyEvent.VK_0;
            if (selectedRow >= 0 && selectedRow < 9 &&
                selectedCol >= 0 && selectedCol < 9 &&
                grid[selectedRow][selectedCol].original == 0) {
                if (isValidPlacement(selectedRow, selectedCol, num)) {
                    grid[selectedRow][selectedCol].value = num;
                    System.out.println("Placed " + num + " at row=" + selectedRow + ", col=" + selectedCol);
                }
            }
        }

        // Delete/Backspace
        if (keyCode == KeyEvent.VK_DELETE || keyCode == KeyEvent.VK_BACK_SPACE) {
            if (selectedRow >= 0 && selectedCol >= 0) {
                if (grid[selectedRow][selectedCol].original == 0) {
                    grid[selectedRow][selectedCol].value = 0;
                }
            }
        }

        // Reset game (R)
        if (keyCode == KeyEvent.VK_R) {
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    if (grid[i][j].original == 0) {
                        grid[i][j].value = 0;
                    }
                }
            }
            mistakes = 0;
            isWon = 0;
        }

        // New game (W)
        if (keyCode == KeyEvent.VK_W) {
            initializeGame();
        }

        // Check for win
        if (isBoardComplete()) {
            isWon = 1;
        }

        repaint();
    }

    boolean isValidPlacement(int row, int col, int num) {
        // Check row
        for (int i = 0; i < 9; i++) {
            if (grid[row][i].value == num) return false;
        }

        // Check column
        for (int i = 0; i < 9; i++) {
            if (grid[i][col].value == num) return false;
        }

        // Check 3x3 box
        int boxRow = (row / 3) * 3;
        int boxCol = (col / 3) * 3;
        for (int i = boxRow; i < boxRow + 3; i++) {
            for (int j = boxCol; j < boxCol + 3; j++) {
                if (grid[i][j].value == num) return false;
            }
        }

        return true;
    }

    void clearBoard(int[][] tempGrid) {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                tempGrid[i][j] = 0;
            }
        }
    }

    boolean solveSudoku(int[][] tempGrid, int row, int col) {
        if (row == 9) return true;

        int nextRow = row;
        int nextCol = col + 1;
        if (nextCol == 9) {
            nextRow++;
            nextCol = 0;
        }

        if (tempGrid[row][col] != 0) {
            return solveSudoku(tempGrid, nextRow, nextCol);
        }

        for (int num = 1; num <= 9; num++) {
            boolean valid = true;

            // Check row
            for (int i = 0; i < 9; i++) {
                if (tempGrid[row][i] == num) {
                    valid = false;
                    break;
                }
            }

            // Check column
            if (valid) {
                for (int i = 0; i < 9; i++) {
                    if (tempGrid[i][col] == num) {
                        valid = false;
                        break;
                    }
                }
            }

            // Check 3x3 box
            if (valid) {
                int boxRow = (row / 3) * 3;
                int boxCol = (col / 3) * 3;
                for (int i = boxRow; i < boxRow + 3; i++) {
                    for (int j = boxCol; j < boxCol + 3; j++) {
                        if (tempGrid[i][j] == num) {
                            valid = false;
                            break;
                        }
                    }
                }
            }

            if (valid) {
                tempGrid[row][col] = num;
                if (solveSudoku(tempGrid, nextRow, nextCol)) {
                    return true;
                }
                tempGrid[row][col] = 0;
            }
        }

        return false;
    }

    void removeNumbers(int[][] tempGrid, int difficulty) {
        int cellsToRemove = 40 + (difficulty * 10);
        int removed = 0;

        while (removed < cellsToRemove) {
            int row = rand.nextInt(9);
            int col = rand.nextInt(9);

            if (tempGrid[row][col] != 0) {
                tempGrid[row][col] = 0;
                removed++;
            }
        }
    }

    void generatePuzzle() {
        int[][] tempGrid = new int[9][9];
        clearBoard(tempGrid);

        // Fill diagonal 3x3 boxes with random numbers
        for (int box = 0; box < 3; box++) {
            int[] nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            // Fisher-Yates shuffle
            for (int i = 8; i > 0; i--) {
                int j = rand.nextInt(i + 1);
                int temp = nums[i];
                nums[i] = nums[j];
                nums[j] = temp;
            }

            int idx = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    tempGrid[box * 3 + i][box * 3 + j] = nums[idx++];
                }
            }
        }

        // Solve the puzzle
        solveSudoku(tempGrid, 0, 0);

        // Copy to game grid
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                grid[i][j].value = tempGrid[i][j];
                grid[i][j].original = tempGrid[i][j];
            }
        }

        // Remove numbers (medium difficulty)
        removeNumbers(tempGrid, 1);

        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                grid[i][j].value = tempGrid[i][j];
                if (tempGrid[i][j] != 0) {
                    grid[i][j].original = tempGrid[i][j];
                } else {
                    grid[i][j].original = 0;
                }
            }
        }
    }

    void initializeGame() {
        selectedRow = -1;
        selectedCol = -1;
        isWon = 0;
        mistakes = 0;

        generatePuzzle();
    }

    boolean isBoardComplete() {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (grid[i][j].value == 0) return false;
            }
        }
        return true;
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        // Draw grid cells
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                int x = col * CELL_SIZE;
                int y = row * CELL_SIZE;

                Color cellColor = Color.WHITE;

                // Highlight selected cell
                if (selectedRow == row && selectedCol == col) {
                    cellColor = new Color(200, 220, 255); // Light blue
                }
                // Highlight row and column of selected cell
                else if (selectedRow == row || selectedCol == col) {
                    cellColor = new Color(240, 240, 240); // Light gray
                }

                g2d.setColor(cellColor);
                g2d.fillRect(x, y, CELL_SIZE, CELL_SIZE);

                g2d.setColor(Color.BLACK);
                g2d.setStroke(new BasicStroke(1));
                g2d.drawRect(x, y, CELL_SIZE, CELL_SIZE);

                // Draw cell value
                if (grid[row][col].value > 0) {
                    Color textColor = Color.BLACK;
                    // Different color for original numbers
                    if (grid[row][col].original > 0) {
                        textColor = Color.DARK_GRAY;
                    }

                    g2d.setColor(textColor);
                    g2d.setFont(new Font("Arial", Font.BOLD, 20));
                    FontMetrics fm = g2d.getFontMetrics();
                    String text = String.valueOf(grid[row][col].value);
                    int textX = x + (CELL_SIZE - fm.stringWidth(text)) / 2;
                    int textY = y + ((CELL_SIZE - fm.getHeight()) / 2) + fm.getAscent();
                    g2d.drawString(text, textX, textY);
                }
            }
        }

        // Draw thick lines for 3x3 boxes
        g2d.setColor(Color.BLACK);
        g2d.setStroke(new BasicStroke(3));
        g2d.drawLine(180, 0, 180, 540);
        g2d.drawLine(360, 0, 360, 540);
        g2d.drawLine(0, 180, 540, 180);
        g2d.drawLine(0, 360, 540, 360);

        // Draw UI at bottom
        g2d.setColor(Color.DARK_GRAY);
        g2d.fillRect(0, 540, PANEL_WIDTH, 60);

        g2d.setColor(Color.WHITE);
        g2d.setFont(new Font("Arial", Font.PLAIN, 15));
        g2d.drawString("1-9: Enter | DEL: Clear | R: Reset | W: New", 10, 560);

        if (isWon == 1) {
            g2d.setColor(Color.YELLOW);
            g2d.setFont(new Font("Arial", Font.BOLD, 25));
            g2d.drawString("YOU WIN!", 220, 565);
        }

        g2d.setColor(Color.WHITE);
        g2d.setFont(new Font("Arial", Font.PLAIN, 15));
        g2d.drawString("Mistakes: " + mistakes, PANEL_WIDTH - 150, 560);
        g2d.drawString("Selected: " + selectedRow + "," + selectedCol, 10, 580);
    }
}
