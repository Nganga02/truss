#include "truss-render.h"
#include "truss-io.h"



int main(void)
{
    struct truss *truss;
    if ((truss = read_truss(stdin)) == NULL)
        return EXIT_FAILURE;
    fprintf(stderr, "Rendering truss\n");
    render_truss(truss);
    fprintf(stderr, "Writing the truss file\n");
    write_truss(truss);
    free_truss(truss);
    return EXIT_SUCCESS;
}