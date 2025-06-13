%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 % THE FOLLOWING CODE WAS USED TO CONVERT FROM CONTINUOUS TO DISCRETE TIME %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

numerator = [-0.2588675];
denominator = [1 0 0];

G_s = tf(numerator, denominator);
%sampling period
T = 0.0239;
%0.0239
method = 'zoh';  %zero order hold

G_z = c2d(G_s, T, method);

format short
disp('Continuous-time transfer function G(s):');
disp(G_s);
disp('Discrete-time transfer function G(z):');
disp(G_z);