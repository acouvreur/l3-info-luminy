
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "systeme.h"


/**********************************************************
** Demarrage du systeme
***********************************************************/

static PSW systeme_init(void) {
	PSW cpu;

	printf("Booting.\n");
	/*** creation d'un programme ***/
	make_inst(0, INST_SUB, 2, 2, -1000); /* R2 -= R2-1000 */
	make_inst(1, INST_ADD, 1, 2, 500);   /* R1 += R2+500 */
	make_inst(2, INST_ADD, 0, 2, 200);   /* R0 += R2+200 */
	make_inst(3, INST_ADD, 0, 1, 100);   /* R0 += R1+100 */

	/*** valeur initiale du PSW ***/
	memset (&cpu, 0, sizeof(cpu));
	cpu.PC = 0;
	cpu.SB = 0;
	cpu.SS = 20;

	return cpu;
}


/**********************************************************
** Simulation du systeme (mode systeme)
***********************************************************/

PSW systeme(PSW m) {

	int i;
	printf("interruption code %d\n", m.IN);

	switch (m.IN) {
		case INT_INIT:
			return (systeme_init());
		case INT_SEGV:
			exit(-1);
			break;
		case INT_TRACE:
			printf("Affichage des registres PC et DR:\n");
			printf("PC:\t%d\n", m.PC);
			for(i = 0; i < 8; i++){
				printf("DR[%d]:\t%d\n", i, m.DR[i]);
			}
			break;
		case INT_INST:
			exit(-1);
			break;
	}
	return m;
}