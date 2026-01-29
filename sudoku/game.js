class Sudoku {
    constructor(size = 9) {
        this.size = size; // 9x9 Sudoku
        this.grid = this.createGrid();
    }

    // Create a 9x9 Sudoku grid filled with zeros
    createGrid() {
        return Array.from({ length: this.size }, () => Array(this.size).fill(0));
    }

    // Fill the grid with a valid Sudoku arrangement
    generate() {
        this.fillGrid();
        this.removeNumbers();
        return this.grid;
    }

    // Fill grid using backtracking algorithm
    fillGrid() {
        for (let row = 0; row < this.size; row++) {
            for (let col = 0; col < this.size; col++) {
                if (this.grid[row][col] === 0) {
                    const nums = this.shuffle([...Array(this.size).keys()].map(x => x + 1));
                    for (let num of nums) {
                        if (this.isValid(num, row, col)) {
                            this.grid[row][col] = num;
                            if (this.fillGrid()) {
                                return true;
                            }
                            this.grid[row][col] = 0; // backtrack
                        }
                    }
                    return false; // No solution found
                }
            }
        }
        return true; // Solved
    }

    // Randomly shuffle an array
    shuffle(array) {
        for (let i = array.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            [array[i], array[j]] = [array[j], array[i]];
        }
        return array;
    }

    // Check if placing num at (row, col) is valid
    isValid(num, row, col) {
        // Check row and column
        for (let i = 0; i < this.size; i++) {
            if (this.grid[row][i] === num || this.grid[i][col] === num) {
                return false;
            }
        }

        // Check the 3x3 box
        const boxRowStart = Math.floor(row / 3) * 3;
        const boxColStart = Math.floor(col / 3) * 3;
        for (let i = boxRowStart; i < boxRowStart + 3; i++) {
            for (let j = boxColStart; j < boxColStart + 3; j++) {
                if (this.grid[i][j] === num) {
                    return false;
                }
            }
        }

        return true;
    }

    // Remove numbers to create the puzzle
    removeNumbers() {
        let count = Math.floor(this.size * this.size * 0.6); // Remove about 60%
        while (count > 0) {
            const row = Math.floor(Math.random() * this.size);
            const col = Math.floor(Math.random() * this.size);
            if (this.grid[row][col] !== 0) {
                this.grid[row][col] = 0;
                count--;
            }
        }
    }

    // Print the Sudoku grid
    printGrid() {
        console.log(this.grid.map(row => row.join(' ')).join('\n'));
    }
}

// Example usage
const sudoku = new Sudoku();
sudoku.generate();
sudoku.printGrid();
