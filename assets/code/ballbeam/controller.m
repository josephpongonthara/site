%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   % THE FOLLOWING CODE WAS USED TO OBTAIN THE CONTINUOUS TIME CONTROLLERS %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
verbose = true
% set time step
T = 0.239;

%% Plant Poles and Coefficients in its Partial Fraction Decomposition

stableRealPlantPoles = [];
stableComplexPlantPoles = [];
unstablePlantPoles = [1];

% Note: we assume that the final unstable plant poles is z=1

stablePlantPoles = [stableRealPlantPoles stableComplexPlantPoles];
qs = [stablePlantPoles unstablePlantPoles];

% coefficents go in order of the poles, and include both c_n and c_(n+1) for
%       the pole at z=1
cs = [-0.0074 -0.0148];

% Note: we assume that the final two coefficients in cs are for z=1 and
%      (z-1)^2

n = length(qs);
nhat = length(stablePlantPoles);
nreal = length(stableRealPlantPoles);
ncomplex = length(stableComplexPlantPoles);


%% Poles Chosen in the Simple Pole Approximation of W[z]

realWPoles = [0.598 0.74 0.82];
%0.8562 0.7589
complexWPoles = [0.598+(9.8e-14)*sqrt(-1) 0.598-(9.8e-14)*sqrt(-1)];
%{
0.8562+0.3412*sqrt(-1) 0.8562-0.3412*sqrt(-1), ...
0.7589+0.2254*sqrt(-1) 0.7589-0.2254*sqrt(-1), ...
0.8525+0.4492*sqrt(-1) 0.8525-0.4492*sqrt(-1)
%}
ps = [realWPoles complexWPoles]

mreal = length(realWPoles);
mcomplex = length(complexWPoles);
m = length(ps);

%% Calculation of alpha, beta, gamma, and gamma hat

alpha = zeros(m);

for i=1:m
    for k=1:n
        alpha(i,i) = alpha(i,i) + cs(k)/(ps(i)-qs(k));
    end
    alpha(i,i) = alpha(i,i) + cs(n+1)/((ps(i)-1)^2);
end

beta = zeros(n+1,m);

for i=1:m
    for k=1:n
        beta(k,i) = cs(k)/(qs(k)-ps(i));
    end
    beta(n,i) = beta(n,i) - cs(n+1)/((1-ps(i))^2);
    beta(n+1,i) = cs(n+1)/(1-ps(i));
end

gamma = zeros(n+1-nhat,m);

for i=1:m
    for j=(nhat+1):n
        gamma(j-nhat,i) = cs(j)/(qs(j)-ps(i));
    end
    gamma(n-nhat,i) = gamma(n-nhat,i) - cs(n+1)/((1-ps(i))^2);
    gamma(n+1-nhat,i) = cs(n+1)/(1-ps(i));
end

gammaHat = zeros(n+1-nhat,nhat);

for k=1:nhat
    for j=(nhat+1):n
        gammaHat(j-nhat,k) = cs(j)/(qs(j)-qs(k));
    end
    gammaHat(n-nhat,k) = gammaHat(n-nhat,k) - cs(n+1)/((1-qs(k))^2);
    gammaHat(n+1-nhat,k) = cs(n+1)/(1-qs(k));
end

%% Determination of A and b matrices for IOP equations

A = [alpha eye(m) zeros(m,nhat);
     beta [zeros(nhat,m) eye(nhat);
           zeros(n+1-nhat,m+nhat)];
     zeros(n+1-nhat,m) gamma gammaHat];

b = [zeros(m+n+1,1);
     -cs((nhat+1):end)'];

%% Determination of step response matrices

% time horizon
K = 1000;

step_ry = zeros(K,m+nhat);

for k=1:K
    for i=1:m
        step_ry(k,i) = -(1-ps(i)^k)/(1-ps(i));
    end
    for j=1:nhat
        step_ry(k,m+j) = -(1-qs(j)^k)/(1-qs(j));
    end
end

step_ru = zeros(K,m);

for k=1:K
    for i=1:m
        step_ru(k,i) = (1-ps(i)^k)/(1-ps(i));
    end
end

%% Determination of steady state vector

steadyState = zeros(1,m+nhat);

for i=1:m
    steadyState(i) = 1/(1-ps(i));
end

for k=1:nhat
    steadyState(m+k) = 1/(1-qs(k));
end


%% Defining the variables for the optimization

wreal = sdpvar(mreal,1,'full');
wcomplex = sdpvar(mcomplex/2,1,'full','complex');
w = wreal;
for i=1:(mcomplex/2)
    w = [w;
         wcomplex(i);
         conj(wcomplex(i))];
end

xreal = sdpvar(mreal,1,'full');
xcomplex = sdpvar(mcomplex/2,1,'full','complex');
x = xreal;
for i=1:(mcomplex/2)
    x = [x;
         xcomplex(i);
         conj(xcomplex(i))];
end

xhatreal = sdpvar(nreal,1,'full');
xhatcomplex = sdpvar(ncomplex/2,1,'full','complex');
xhat = xhatreal;
for i=1:(ncomplex/2)
    xhat = [xhat;
            xhatcomplex(i);
            conj(xhatcomplex(i))];
end


%% Defining the objective function and constraints for the optimization

Objective = 0;
%Objective = max(step_ru*w);

% IOP constraint
Constraints = [A*[w;x;xhat] == b];

% input saturation constraint
Constraints = [Constraints,
               max(step_ru*w) <= 7,
               min(step_ru*w) >= -7];

% steady state constraint
Constraints = [Constraints, 
                1 + steadyState*[x;xhat] == 0];


% overshoot constraint
Constraints = [Constraints,
               max(step_ry*[x;xhat]) <= 1.45*(-steadyState*[x;xhat])];


% settling time constraint
jhat = 7/T;
Constraints = [Constraints,
               max(step_ry(jhat:end,:)*[x;xhat]) <= 1.02*(-steadyState*[x;xhat]),
               min(step_ry(jhat:end,:)*[x;xhat]) >= 0.98*(-steadyState*[x;xhat])];

%% Solving the optimization problem

% set some options for YALMIP and solver
options = sdpsettings('verbose',1,'solver','mosek');
% solve the problem
sol = optimize(Constraints,Objective,options);

% obtain the solution
wsol = value(w);
xsol = value(x);
xhatsol = value(xhat);

%% Plotting the solution

figure(1)
plot(T*(1:K),step_ry*[xsol;xhatsol]);
xlabel('Time [s]','FontWeight','bold');
ylabel('y[k]','FontWeight','bold');
%fixPlot(1);

figure(2)
plot(T*(1:K),step_ru*wsol);
xlabel('Time [s]','FontWeight','bold');
ylabel('u[k]','FontWeight','bold');
%fixPlot(2);

%%Recovering transfer function
z = tf('z',T);

% Calculate W
W = 0;
for i = 1:m
    W = W + wsol(i) / (z - ps(i));
end

% Calculate X
X = 1;
for i = 1:m
    X = X + xsol(i) / (z - ps(i));
end
for k = 1:nhat
    X = X + xhatsol(k) / (z - qs(k));
end

% Remove the imaginary coefficients in W
[num, den] = tfdata(W);
num{1} = real(num{1});
den{1} = real(den{1});
W = tf(num, den, T);
% Remove the imaginary coefficients in X
[num, den] = tfdata(X);
num{1} = real(num{1});
den{1} = real(den{1});
X = tf(num, den, T);
%{
% Calculate G (if desired)
G = 0;
for k = 1:n
    G = G + cs(k) / (z - qs(k));
end
G = G + cs(n+1) / (z - 1)^2;
%}
% Find the poles and zeroes of W and X
W_zpk = zpk(W)
W_zeroes = zero(W)
W_poles = pole(W)
X_zpk = zpk(X)
X_zeroes = zero(X)
X_poles = pole(X)

%{
D_numerator = transpose([transpose(W_zeroes) transpose(X_poles)]);
D_denominator = transpose([transpose(W_poles) transpose(X_zeroes)]);
[numerator_num_terms, numerator_terms] = groupcounts(D_numerator);
[denominator_num_terms, denominator_terms] = groupcounts(D_denominator);
W_gain = W_zpk.K;
X_gain = X_zpk.K;
D_gain = W_gain/X_gain;

% Performing pole-zero cancellations
for i=1:numel(numerator_terms)
    % For each term in the numerator, check how many repetitions it has
    % and then check if there is any to cancel in the denominator
    for j=1:numel(denominator_terms)
        if(numerator_terms(i) == denominator_terms(j))
            if(numerator_num_terms(i) > denominator_num_terms(j))
                % If numerator occurrence is greater, reduce numerator
                % occurrence by number of denominator occurrence and zero
                % the denominator occurrence value
                numerator_num_terms(i) = numerator_num_terms(i) - denominator_num_terms(j);
                denominator_num_terms(j) = 0;
            elseif(numerator_num_terms(i) < denominator_num_terms(j))
                denominator_num_terms(j) = denominator_num_terms(j) - numerator_num_terms(i);
                numerator_num_terms(i) = 0;
            else
                % if both equal and neither greater then cancel all of it
                numerator_num_terms(i) = 0;
                denominator_num_terms(j) = 0;
            end
        end
    end
end

final_numerator_terms = [];
final_denominator_terms = [];

% Producing the simplified controller sets
for i=1:numel(numerator_terms)
    if(numerator_num_terms(i) > 0)
        for j=1:numerator_num_terms(i)
            final_numerator_terms = [final_numerator_terms; numerator_terms(i)];
        end
    end
end

for i=1:numel(denominator_terms)
    if(denominator_num_terms(i) > 0)
        for j=1:denominator_num_terms(i)
            final_denominator_terms = [final_denominator_terms; denominator_terms(i)];
        end
    end
end

% Produce the transfer function with the simplified numerator/denominator
% sets
numerator_tf = tf([1], [1], T)
for i=1:numel(final_numerator_terms)
    current_tf = tf([1 -final_numerator_terms(i)], [1], T);
    numerator_tf = numerator_tf*current_tf;
end
denominator_tf = tf([1], [1], T)
for i=1:numel(final_denominator_terms)
    current_tf = tf([1], [1 -final_denominator_terms(i)], T);
    denominator_tf = denominator_tf*current_tf;
end

final_D_tf = D_gain*numerator_tf*denominator_tf

% Decimal resolution of how many decimals round to consider a part
% negligible
% Same as r_val but for deciding when real/imag parts are useless
decimal_res = 4;
for i=1:numel(final_D_tf.Numerator{1,1})
    if(round(imag(final_D_tf.Numerator{1,1}(i)),decimal_res) == 0)
        final_D_tf.Numerator{1,1}(i) = real(final_D_tf.Numerator{1,1}(i));
    elseif(round(real(final_D_tf.Numerator{1,1}(i)),decimal_res) == 0)
        final_D_tf.Numerator{1,1}(i) = imag(final_D_tf.Numerator{1,1}(i));
    else
        final_D_tf.Numerator{1,1}(i) = final_D_tf.Numerator{1,1}(i);
    end
end

for i=1:numel(final_D_tf.Denominator{1,1})
    if(round(imag(final_D_tf.Denominator{1,1}(i)),decimal_res) == 0)
        final_D_tf.Denominator{1,1}(i) = real(final_D_tf.Denominator{1,1}(i));
    elseif(round(real(final_D_tf.Denominator{1,1}(i)),decimal_res) == 0)
        final_D_tf.Denominator{1,1}(i) = imag(final_D_tf.Denominator{1,1}(i));
    else
        final_D_tf.Denominator{1,1}(i) = final_D_tf.Denominator{1,1}(i);
    end
end

final_D_tf

figure(3)
stability_test = feedback(final_D_tf*G_z, 1)
step(stability_test)
%}