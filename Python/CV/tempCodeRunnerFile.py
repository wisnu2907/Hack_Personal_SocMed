
    # Calibration
    if calibrated:
        # Resize the frame based on the calibration points
        width = int(
            np.linalg.norm(np.array(clicked_points[1]) - np.array(clicked_points[0]))
        )
        height = int(
            np.linalg.norm(np.array(clicked_points[2]) - np.array(clicked_points[1]))
        )

        pts1 = np.float32(clicked_points)
        pts2 = np.float32([[0, height], [width, height], [width, 0], [0, 0]])

        matrix = cv2.getPerspectiveTransform(pts1, pts2)
        frame = cv2.warpPerspective(frame, matrix, (width, height))