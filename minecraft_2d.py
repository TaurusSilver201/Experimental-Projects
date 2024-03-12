import pygame
import random
import math
import socket
import threading
import pickle
import noise

# Initialize Pygame
pygame.init()

# Set up the game window
WINDOW_WIDTH = 1200
WINDOW_HEIGHT = 800
window = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
pygame.display.set_caption("2D Minecraft")

# Define colors
BLACK = (0, 0, 0)
BROWN = (139, 69, 19)
GREEN = (0, 128, 0)
BLUE = (0, 0, 255)
WHITE = (255, 255, 255)
RED = (255, 0, 0)

# Define block types
AIR = 0
DIRT = 1
GRASS = 2
STONE = 3
WOOD = 4
COAL = 5
IRON = 6
DIAMOND = 7
BEDROCK = 8
WATER = 9
SAND = 10

# Define block size
BLOCK_SIZE = 32

# Define player
player_x = WINDOW_WIDTH // 2
player_y = WINDOW_HEIGHT // 2
player_size = 16
player_health = 100

# Define world
world_width = WINDOW_WIDTH // BLOCK_SIZE
world_height = WINDOW_HEIGHT // BLOCK_SIZE
world = [[AIR for _ in range(world_width)] for _ in range(world_height)]

# Generate terrain
scale = 100.0
octaves = 6
persistence = 0.5
lacunarity = 2.0

for x in range(world_width):
    for y in range(world_height):
        terrain_height = noise.pnoise2(x / scale, y / scale, octaves, persistence, lacunarity)
        terrain_height = (terrain_height + 1) / 2
        terrain_height *= world_height // 2
        if y < terrain_height:
            if y == terrain_height - 1:
                world[y][x] = GRASS
            elif y > terrain_height - 4:
                world[y][x] = DIRT
            else:
                world[y][x] = STONE

# Add water and sand
for x in range(world_width):
    for y in range(world_height):
        if world[y][x] == AIR:
            if y == world_height - 1:
                world[y][x] = WATER
            elif random.random() < 0.1:
                world[y][x] = SAND

# Add bedrock layer
for x in range(world_width):
    world[world_height - 1][x] = BEDROCK

# Define inventory
inventory = {
    DIRT: 0,
    GRASS: 0,
    STONE: 0,
    WOOD: 0,
    COAL: 0,
    IRON: 0,
    DIAMOND: 0,
    SAND: 0,
}

# Define crafting recipes
crafting_recipes = {
    WOOD: [(WOOD, 2)],
    COAL: [(STONE, 2)],
    IRON: [(STONE, 4), (COAL, 1)],
    DIAMOND: [(IRON, 2), (COAL, 1)],
}

# Define tools
tools = {
    "wood_pickaxe": {
        "recipe": [(WOOD, 3), (STONE, 2)],
        "mining_power": 2,
    },
    "stone_pickaxe": {
        "recipe": [(STONE, 3), (WOOD, 2)],
        "mining_power": 4,
    },
    "iron_pickaxe": {
        "recipe": [(IRON, 3), (WOOD, 2)],
        "mining_power": 6,
    },
    "diamond_pickaxe": {
        "recipe": [(DIAMOND, 3), (WOOD, 2)],
        "mining_power": 8,
    },
}

# Define enemies
enemies = []
for _ in range(10):
    enemy_x = random.randint(0, world_width - 1) * BLOCK_SIZE
    enemy_y = random.randint(0, world_height - 1) * BLOCK_SIZE
    enemies.append({"x": enemy_x, "y": enemy_y, "health": 50})

# Define day/night cycle
day_length = 30 * 60  # 30 minutes
night_length = 15 * 60  # 15 minutes
time_of_day = 0
is_day = True

# Define multiplayer
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(("localhost", 5555))
server_socket.listen(5)
clients = []

# Game loop
running = True
clock = pygame.time.Clock()
while running:
    # Handle events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            # Get the block position
            mouse_x, mouse_y = event.pos
            block_x = mouse_x // BLOCK_SIZE
            block_y = mouse_y // BLOCK_SIZE

            # Break block
            if event.button == 1:
                block_type = world[block_y][block_x]
                if block_type != AIR and block_type != BEDROCK:
                    inventory[block_type] += 1
                    world[block_y][block_x] = AIR

            # Place block
            elif event.button == 3:
                block_type = None
                for item_type, count in inventory.items():
                    if count > 0:
                        block_type = item_type
                        break
                if block_type is not None:
                    inventory[block_type] -= 1
                    world[block_y][block_x] = block_type

    # Move player
    keys = pygame.key.get_pressed()
    if keys[pygame.K_LEFT]:
        player_x -= 5
    if keys[pygame.K_RIGHT]:
        player_x += 5
    if keys[pygame.K_UP]:
        player_y -= 5
    if keys[pygame.K_DOWN]:
        player_y += 5

    # Update enemies
    for enemy in enemies:
        dx = enemy["x"] - player_x
        dy = enemy["y"] - player_y
        distance = math.sqrt(dx ** 2 + dy ** 2)
        if distance < 200:
            if dx > 0:
                enemy["x"] += 1
            else:
                enemy["x"] -= 1
            if dy > 0:
                enemy["y"] += 1
            else:
                enemy["y"] -= 1

    # Detect collisions with enemies
    for enemy in enemies:
        if (
            abs(enemy["x"] - player_x) < BLOCK_SIZE
            and abs(enemy["y"] - player_y) < BLOCK_SIZE
        ):
            player_health -= 10
            if player_health <= 0:
                running = False

    # Update day/night cycle
    time_of_day = (time_of_day + 1) % (day_length + night_length)
    if time_of_day == 0:
        is_day = not is_day

    # Draw world
    for y in range(world_height):
        for x in range(world_width):
            block_type = world[y][x]
            if block_type == AIR:
                block_color = BLACK
            elif block_type == DIRT:
                block_color = BROWN
            elif block_type == GRASS:
                block_color = GREEN
            elif block_type == STONE:
                block_color = (128, 128, 128)
            elif block_type == WOOD:
                block_color = (153, 76, 0)
            elif block_type == COAL:
                block_color = (64, 64, 64)
            elif block_type == IRON:
                block_color = (192, 192, 192)
            elif block_type == DIAMOND:
                block_color = (0, 255, 255)
            elif block_type == BEDROCK:
                block_color = (64, 64, 64)
            elif block_type == WATER:
                block_color = BLUE
            elif block_type == SAND:
                block_color = (255, 255, 128)
            pygame.draw.rect(window, block_color, (x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE))

    # Draw player
    pygame.draw.rect(window, BLACK, (player_x - player_size // 2, player_y - player_size // 2, player_size, player_size))

    # Draw enemies
    for enemy in enemies:
        pygame.draw.rect(window, RED, (enemy["x"] - 8, enemy["y"] - 8, 16, 16))

    # Draw inventory
    inventory_x = 10
    inventory_y = WINDOW_HEIGHT - 100
    for item_type, count in inventory.items():
        if count > 0:
            item_name = None
            item_color = None
            if item_type == DIRT:
                item_name = "Dirt"
                item_color = BROWN
            elif item_type == GRASS:
                item_name = "Grass"
                item_color = GREEN
            elif item_type == STONE:
                item_name = "Stone"
                item_color = (128, 128, 128)
            elif item_type == WOOD:
                item_name = "Wood"
                item_color = (153, 76, 0)
            elif item_type == COAL:
                item_name = "Coal"
                item_color = (64, 64, 64)
            elif item_type == IRON:
                item_name = "Iron"
                item_color = (192, 192, 192)
            elif item_type == DIAMOND:
                item_name = "Diamond"
                item_color = (0, 255, 255)
            elif item_type == SAND:
                item_name = "Sand"
                item_color = (255, 255, 128)

            pygame.draw.rect(window, item_color, (inventory_x, inventory_y, BLOCK_SIZE, BLOCK_SIZE))
            font = pygame.font.Font(None, 24)
            text = font.render(f"{item_name}: {count}", True, BLACK)
            window.blit(text, (inventory_x + BLOCK_SIZE + 10, inventory_y))
            inventory_x += BLOCK_SIZE * 2 + 20

    # Draw player health
    font = pygame.font.Font(None, 36)
    text = font.render(f"Health: {player_health}", True, WHITE)
    window.blit(text, (10, 10))

    # Draw day/night indicator
    if is_day:
        pygame.draw.circle(window, (255, 255, 0), (WINDOW_WIDTH - 50, 50), 25)
    else:
        pygame.draw.circle(window, (128, 128, 128), (WINDOW_WIDTH - 50, 50), 25)

    # Update display
    pygame.display.update()
    clock.tick(60)  # Limit to 60 FPS

    # Multiplayer
    connection, address = server_socket.accept()
    clients.append(connection)

    def handle_client(client):
        while True:
            try:
                data = client.recv(1024)
                if data:
                    # Handle incoming data from the client
                    # For example, update the world based on the received data
                    client_world = pickle.loads(data)
                    for y in range(world_height):
                        for x in range(world_width):
                            world[y][x] = client_world[y][x]
            except:
                clients.remove(client)
                client.close()
                break

    threading.Thread(target=handle_client, args=(connection,)).start()

    # Save world
    with open("world.dat", "wb") as f:
        pickle.dump(world, f)

# Quit Pygame
pygame.quit()
