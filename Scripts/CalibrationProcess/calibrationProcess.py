import cv2
import numpy as np
import glob

# Parámetros del tablero (cambia según tu patrón)
CHECKERBOARD = (10, 7)  # esquinas interiores por fila y columna
SQUARE_SIZE = 1.5      # tamaño real de una casilla (en cm, mm o lo que uses)

# Preparar puntos 3D del patrón (por ejemplo, (0,0,0), (1,0,0), ...)
objp = np.zeros((CHECKERBOARD[0]*CHECKERBOARD[1], 3), np.float32)
objp[:, :2] = np.mgrid[0:CHECKERBOARD[0], 0:CHECKERBOARD[1]].T.reshape(-1, 2)
objp *= SQUARE_SIZE

objpoints = []  # puntos 3D reales
imgpoints = []  # puntos 2D detectados en las imágenes

images = glob.glob("resources/*.jpeg")
print(f"Imágenes encontradas: {len(images)}")
for fname in images:
    img = cv2.imread(fname)
    if img is None:
        print(f"[ERROR] No se pudo leer imagen: {fname}")
        continue
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # Buscar el patrón del tablero
    ret, corners = cv2.findChessboardCorners(gray, CHECKERBOARD, None)

    if ret:
        print(f"Patron encontrado en: {fname}")
        objpoints.append(objp)
        corners2 = cv2.cornerSubPix(
            gray, corners, (11, 11), (-1, -1),
            criteria=(cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
        )
        imgpoints.append(corners2)

        # Dibujar y mostrar esquinas detectadas (opcional)
        cv2.drawChessboardCorners(img, CHECKERBOARD, corners2, ret)
        cv2.imshow('Deteccion', img)
        cv2.waitKey(100)
    else:
        print(f"[WARN] No se encontro patron en: {fname}")
cv2.destroyAllWindows()

# Calibrar la cámara
ret, cameraMatrix, distCoeffs, rvecs, tvecs = cv2.calibrateCamera(
    objpoints, imgpoints, gray.shape[::-1], None, None
)

print("[INFO] Matriz de camara:\n", cameraMatrix)
print("[INFO] Coeficientes de distorsion:\n", distCoeffs)
print(f"[INFO] Error medio de reproyeccion: {ret}")

# Error de reproyección (opcional)
mean_error = 0
for i in range(len(objpoints)):
    imgpoints2, _ = cv2.projectPoints(objpoints[i], rvecs[i], tvecs[i], cameraMatrix, distCoeffs)
    error = cv2.norm(imgpoints[i], imgpoints2, cv2.NORM_L2) / len(imgpoints2)
    mean_error += error

print(f"[INFO] Error medio de reproyeccion calculado: {mean_error / len(objpoints)}")
