//package fr.licinfo;
//
///**
// * Created by sebastientosello on 23/11/2016.
// */
//
//import java.util.Arrays;
//
///**
// * Finds the maximum flow in a flow network.
// * @param E neighbour lists
// * @param C capacity matrix (must be n by n)
// * @param s source
// * @param t sink
// * @return maximum flow
// */
//public class EdmondsKarp {
//    public static int edmondsKarp(int[][] E, int[][] C, int s, int t) {
//        int n = C.length;
//        // Residual capacity from u to v is C[u][v] - F[u][v]
//        int[][] F = new int[n][n];
//        while (true) {
//            int[] P = new int[n]; // Parent table
//            Arrays.fill(P, -1);
//            P[s] = s;
//            int[] M = new int[n]; // Capacity of path to node
//            M[s] = Integer.MAX_VALUE;
//            // BFS queue
//            Queue<Integer> Q = new LinkedList<Integer>();
//            Q.offer(s);
//            LOOP:
//            while (!Q.isEmpty()) {
//                int u = Q.poll();
//                for (int v : E[u]) {
//                    // There is available capacity,
//                    // and v is not seen before in search
//                    if (C[u][v] - F[u][v] > 0 && P[v] == -1) {
//                        P[v] = u;
//                        M[v] = Math.min(M[u], C[u][v] - F[u][v]);
//                        if (v != t)
//                            Q.offer(v);
//                        else {
//                            // Backtrack search, and write flow
//                            while (P[v] != v) {
//                                u = P[v];
//                                F[u][v] += M[t];
//                                F[v][u] -= M[t];
//                                v = u;
//                            }
//                            break LOOP;
//                        }
//                    }
//                }
//            }
//            if (P[t] == -1) { // We did not find a path to t
//                int sum = 0;
//                for (int x : F[s])
//                    sum += x;
//                return sum;
//            }
//        }
//    }
//}