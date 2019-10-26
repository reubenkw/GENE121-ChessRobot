import chess
import chess.engine

engine = chess.engine.SimpleEngine.popen_uci("stockfish.exe")

board = chess.Board()
while not board.is_game_over():
    result = engine.play(board, chess.engine.Limit(time=0.100))
    board.push(result.move)
print(board)
engine.quit()
