import time
import chess
import chess.engine

folderAddress = "C:/Users/Reube/Desktop/GENE121-ChessRobot/"

def waitForInFile(desiredFirstElement):
    firstElement = str(desiredFirstElement + 1)      #insures initialization does not cause equality
    while int(firstElement) != desiredFirstElement:
        time.sleep(2)
        with open(folderAddress + "IPC/IPC_CPP_to_PY.txt","r") as fin:
            firstElement = fin.readline()
    time.sleep(1)

outNum = 0
with open(folderAddress + "IPC/IPC_PY_to_CPP.txt","w") as fout:
    fout.write(str(outNum))
    outNum += 1

print("Waiting for File Sync")

inNum = 0
waitForInFile(inNum)
inNum += 1

print("Files Synced")

engine = chess.engine.SimpleEngine.popen_uci(folderAddress + "stockfish.exe")
crntBoard = chess.Board()

while not crntBoard.is_game_over():

    outCmd = "none"

    print("Waiting for file")

    waitForInFile(inNum)
    inNum += 1

    print("File recieved")

    with open(folderAddress + "IPC/IPC_CPP_to_PY.txt","r") as fin:
        fin.readline()
        fileInput = fin.readline()
        print(fileInput)
        UserMove = chess.Move.from_uci(str(fileInput))
        print("Human move:", UserMove)

    if UserMove in crntBoard.legal_moves:
        crntBoard.push(UserMove)

        if crntBoard.is_game_over():
            outCmd = "Human_wins"
            if crntBoard.is_stalemate():
                outCmd = "Draw"
            with open(folderAddress + "IPC/IPC_PY_to_CPP.txt","w") as fout:
                fout.write(str(outNum) + "\n" + outCmd)
            outNum += 1
            print(outCmd)
        else:

            result = engine.play(crntBoard, chess.engine.Limit(time=2))
            crntBoard.push(result.move)

            if crntBoard.is_game_over():
                outCmd = "Computer_wins"
                if crntBoard.is_stalemate():
                    outCmd = "Draw"
                with open(folderAddress + "IPC/IPC_PY_to_CPP.txt","w") as fout:
                    fout.write(str(outNum) + "\n" + outCmd)
                outNum += 1
                print(outCmd)
            else:
                with open(folderAddress + "IPC/IPC_PY_to_CPP.txt","w") as fout:
                    fout.write(str(outNum) + "\n" + outCmd + "\n" + str(result.move) + "\n")

                    for m in crntBoard.legal_moves:
                        fout.write(str(m) + "\n")
                outNum += 1
                print("Computer move:", result.move)

    else:
        outCmd = "Illegal_Move"
        with open(folderAddress + "IPC/IPC_PY_to_CPP.txt","w") as fout:
            fout.write(str(outNum) + "\n" + outCmd)
        outNum += 1
        print(outCmd)

    print(crntBoard)

engine.quit()
exit()
