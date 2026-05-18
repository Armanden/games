// Maze Game with GUI in Go using Ebiten
//
// Install dependencies:
//   go get github.com/hajimehoshi/ebiten/v2
//
// Run:
//   go run main.go

package main

import (
	"image/color"
	"log"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
)

const (
	screenWidth  = 640
	screenHeight = 640
	tileSize     = 64
)

type Game struct {
	maze   []string
	player Position
	exit   Position
	won    bool
}

type Position struct {
	x int
	y int
}

func NewGame() *Game {
	maze := []string{
		"##########",
		"#P     # #",
		"# ### ## #",
		"#   #    #",
		"### #### #",
		"#     #  #",
		"# ### ## #",
		"# #      #",
		"#   ####E#",
		"##########",
	}

	var player Position
	var exit Position

	for y, row := range maze {
		for x, cell := range row {
			if cell == 'P' {
				player = Position{x, y}
			}
			if cell == 'E' {
				exit = Position{x, y}
			}
		}
	}

	return &Game{
		maze:   maze,
		player: player,
		exit:   exit,
	}
}

func (g *Game) Update() error {
	if g.won {
		return nil
	}

	if ebiten.IsKeyPressed(ebiten.KeyW) {
		g.move(0, -1)
	}
	if ebiten.IsKeyPressed(ebiten.KeyS) {
		g.move(0, 1)
	}
	if ebiten.IsKeyPressed(ebiten.KeyA) {
		g.move(-1, 0)
	}
	if ebiten.IsKeyPressed(ebiten.KeyD) {
		g.move(1, 0)
	}

	if g.player == g.exit {
		g.won = true
	}

	return nil
}

func (g *Game) move(dx, dy int) {
	newX := g.player.x + dx
	newY := g.player.y + dy

	if newY < 0 || newY >= len(g.maze) {
		return
	}

	if newX < 0 || newX >= len(g.maze[0]) {
		return
	}

	target := g.maze[newY][newX]

	if target == '#' {
		return
	}

	g.player.x = newX
	g.player.y = newY
}

func (g *Game) Draw(screen *ebiten.Image) {
	screen.Fill(color.RGBA{30, 30, 30, 255})

	for y, row := range g.maze {
		for x, cell := range row {

			switch cell {

			// Walls
			case '#':
				ebitenutil.DrawRect(
					screen,
					float64(x*tileSize),
					float64(y*tileSize),
					tileSize,
					tileSize,
					color.RGBA{70, 70, 200, 255},
				)

			// Exit
			case 'E':
				ebitenutil.DrawRect(
					screen,
					float64(x*tileSize),
					float64(y*tileSize),
					tileSize,
					tileSize,
					color.RGBA{0, 200, 0, 255},
				)
			}
		}
	}

	// Player
	ebitenutil.DrawRect(
		screen,
		float64(g.player.x*tileSize),
		float64(g.player.y*tileSize),
		tileSize,
		tileSize,
		color.RGBA{220, 50, 50, 255},
	)

	if g.won {
		ebitenutil.DebugPrint(screen, "YOU ESCAPED!")
	} else {
		ebitenutil.DebugPrint(screen, "WASD to Move")
	}
}

func (g *Game) Layout(outsideWidth, outsideHeight int) (int, int) {
	return screenWidth, screenHeight
}

func main() {
	ebiten.SetWindowSize(screenWidth, screenHeight)
	ebiten.SetWindowTitle("Maze Game")

	game := NewGame()

	if err := ebiten.RunGame(game); err != nil {
		log.Fatal(err)
	}
}
