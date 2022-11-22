% Proyecto Sistemas de Control Avanzados
% Isaac Zambrano

%% Importar los datos
clc
clear all
close all
Datos=load('Lazo_Abierto.csv'); %Cargar los datos del excel y guardarlos en la variable
OUT=Datos(:,1); %Salida del Sistema
IN=Datos(:,2); %Entrada del Sistema (Verificar que este valor este entre 0 y 255, caso contrario *255)
Time=(0:1:length(Datos)-1)'; %Vector de tiempo

%% Graficar los datos
figure(1)
plot(Time,IN); %Se grafica la entrada del sistema
title('Entrada: Altos y Bajos del Microcontrolador')
ylabel('Señal del microcontrolador')
xlabel('Tiempo [s]')
grid on

figure(2)
plot(Time,OUT); %Se grafica la salida del sistema
title('Salida: Temperatura de los transistores')
ylabel('Temperatura [°C]')
xlabel('Tiempo [s]')
grid on

%% Identificación de sistemas
systemIdentification %Se llama un comando de matlab importante para la identificación


%% Función de Transferencia obtenida (Metodo 3)
Gp=tf(d2c(bj22221));%<---Modelo con mejor FIT
num=cell2mat(Gp.numerator);
den=cell2mat(Gp.denominator);
FTmotor=tf(num,den);

Time=(0:1:length(IN)-1)'; %Vector de tiempo
y = lsim(Gp,IN,Time); % system response
figure(1)
plot(y);
hold on;
plot(IN);
plot(OUT);
legend('Estimate','Input','Output');

%% PID Tunner
pidTuner
