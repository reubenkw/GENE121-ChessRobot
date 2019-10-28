import time
import chess
import chess.engine

folderAddress = "C:/Users/Reube/Google Drive/School/University/Term 1A/GENE121/GENE121-ChessRobot/"

def waitForInFile(desiredFirstElement):
    firstElement = str(desiredFirstElement + 1)      #insures initialization does not cause equality
    while int(firstElement) != desiredFirstElement:
        time.sleep(2)
        with open(folderAddress + "IPC/IPC_CPP_to_PY.txt","r") as fin:
            firstElement = fin.readline()
outNum = 0
with open(folderAddress + "IPC/IPC_PY_to_CPP.txt","w") as fout:
    fout.write(str(outNum))
    outNum += 1

inNum = 0
waitForInFile(inNum)
inNum += 1

engine = chess.engine.SimpleEngine.popen_uci(folderAddress + "stockfish.exe")
board = chess.Board()

while not board.is_game_over():
    print(board.legal_moves)

    waitForInFile(inNum)
    inNum += 1

    with open(folderAddress + "IPC/IPC_CPP_to_PY.txt","r") as fin:
        fin.readline()
        UserMove = fin.readline()
    board.push_uci(UserMove)

    result = engine.play(board, chess.engine.Limit(time=2))
    board.push(result.move)

    with open(folderAddress + "IPC/IPC_PY_to_CPP.txt","w") as fout:
        fout.write(str(outNum) + "\n" + str(board))
        outNum += 1

engine.quit()
