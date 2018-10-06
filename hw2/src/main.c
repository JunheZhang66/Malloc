
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "version.h"
#include "global.h"
#include "gradedb.h"
#include "stats.h"
#include "read.h"
#include "write.h"
#include "normal.h"
#include "sort.h"
#include "error.h"
#include "report.h"
/*
 * Course grade computation program
 */

#define REPORT          'r'
#define COLLATE         'c'
#define FREQUENCIES     2
#define QUANTILES       3
#define SUMMARIES       4
#define MOMENTS         5
#define COMPOSITES      6
#define INDIVIDUALS     7
#define HISTOGRAMS      8
#define TABSEP          9
#define ALLOUTPUT      'a'
#define SORTBY         'k'
#define NONAMES        'n'
#define OUTPUT         'o'

static struct option_info {
        unsigned int val;
        char *name;
        char chr;
        int has_arg;
        char *argname;
        char *descr;
} option_table[] = {
 {REPORT,         "report",    'r',      no_argument, NULL,
                  "Process input data and produce specified reports."},
 {COLLATE,        "collate",   'c',      no_argument, NULL,
                  "Collate input data and dump to standard output."},
 {FREQUENCIES,    "freqs",     0,        no_argument, NULL,
                  "Print frequency tables."},
 {QUANTILES,      "quants",    0,        no_argument, NULL,
                  "Print quantile information."},
 {SUMMARIES,      "summaries", 0,        no_argument, NULL,
                  "Print quantile summaries."},
 {MOMENTS,        "stats",     0,        no_argument, NULL,
                  "Print means and standard deviations."},
 {COMPOSITES,     "comps",     0,        no_argument, NULL,
                  "Print students' composite scores."},
 {INDIVIDUALS,    "indivs",    0,        no_argument, NULL,
                  "Print students' individual scores."},
 {HISTOGRAMS,     "histos",    0,        no_argument, NULL,
                  "Print histograms of assignment scores."},
 {TABSEP,         "tabsep",    0,        no_argument, NULL,
                  "Print tab-separated table of student scores."},
 {ALLOUTPUT,      "all",       'a',      no_argument, NULL,
                  "Print all reports."},
 {NONAMES,        "nonames",   'n',      no_argument, NULL,
                  "Suppress printing of students' names."},
 {SORTBY,         "sortby",    'k',      required_argument, "key",
                  "Sort by {name, id, score}."},
 {OUTPUT,         "output",    'o',      required_argument, "outfile",
                  "Write output to file, rather than standard output."}

};

#define NUM_OPTIONS (15)

static char *short_options = "rcank:o:";
static struct option long_options[NUM_OPTIONS];

static void init_options() {
    for(unsigned int i = 0; i < NUM_OPTIONS-1; i++) {
        struct option_info *oip = &option_table[i];
        struct option *op = &long_options[i];
        op->name = oip->name;
        op->has_arg = oip->has_arg;
        op->flag = NULL;
        op->val = oip->val;
    }
    struct option *op = &long_options[14];
    op->name = NULL;
    op->has_arg = 0;
    op->flag = NULL;
    op->val = 0;
}

static int report, collate, freqs, quantiles, summaries, moments,
           scores, composite, histograms, tabsep, nonames, outfile,sortby;

static void usage();

int main(argc, argv)
int argc;
char *argv[];
{
        Course *c;
        Stats *s;
        extern int errors, warnings;
        char optval;
        int (*compare)() = comparename;
        FILE* out_file;
        //FILE* fp;
        fprintf(stderr, BANNER);
        init_options();
        if(argc <= 1) usage(argv[0]);
        while(optind < argc) {
            if((optval = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
                if(((report!=1)&(collate!=1))&(optind!=2)&(optind!=1)){
                        fprintf(stderr, "Either -r or, -c need to be the fisrt option.\n\n");
                        usage(argv[0]);
                    }
                switch(optval) {

                case REPORT: report++; 
                            if(report<=(collate+freqs+quantiles+summaries+moments+scores+composite+histograms+tabsep+nonames+outfile+sortby))
                            {
                                fprintf(stderr,
                                "illegal option received, -r or -c need to be first");
                            
                                 usage(argv[0]);
                            }
                            break;

                case COLLATE: collate++; 
                            if(collate<=(report+freqs+quantiles+summaries+moments+scores+composite+histograms+tabsep+nonames+outfile+sortby))
                            {
                                fprintf(stderr,
                                "illegal option received, -r or -c need to be first");
                            
                                 usage(argv[0]);
                            }
                            break;
                case TABSEP: tabsep++; break;
                case NONAMES: nonames++; break;
                case SORTBY:
                    //if(optarg==NULL){printf("%s","" );}
                    sortby++;
                    
                    if(!strcmp(optarg, "name")){
                        //printf("%s'\n","inside name");
                        compare = comparename;
                    }
                    else if(!strcmp(optarg, "id"))
                        compare = compareid;
                    else if(!strcmp(optarg, "score"))
                        compare = comparescore;
                    else {
                        if(optarg==NULL){
                            fprintf(stderr,
                                "Option -k requires argument from {name, id, score}.\n\n");
                        usage(argv[0]);
                            }
                        fprintf(stderr,
                                "Option -k requires argument from {name, id, score}.\n\n");
                        usage(argv[0]);              
                    }
                    break;

                case OUTPUT: 
                     outfile++;
                     if(optarg!=NULL)
                     {
                            
                     out_file=fopen(optarg,"w");
                     stdout=out_file;
                    
                     }
                     else if(optarg==NULL){
                        fprintf(stderr, "[outfile] argument is required, enter valid text file name");
                            usage(argv[0]);
                            }
                    break;
                case FREQUENCIES: freqs++; break;
                case QUANTILES: quantiles++; break;
                case SUMMARIES: summaries++; break;
                case MOMENTS: moments++; break;
                case COMPOSITES: composite++; break;
                case INDIVIDUALS: scores++; break;
                case HISTOGRAMS: histograms++; break;
                case ALLOUTPUT:
                    freqs++; quantiles++; summaries++; moments++;
                    composite++; scores++; histograms++; tabsep++;
                    break;
                case '?':
                    usage(argv[0]);
                    break;
                default:
                    break;
                }
            } else {
                break;
            }
        }
        if((report>1)|| (collate>1)|| (freqs>1)|| (quantiles>1)|| (summaries>1)|| (moments>1)||
           (scores>1)|| (composite>1)|| (histograms>1)|| (tabsep>1)|| (nonames>1)|| (outfile>1)||(sortby>1)){
        
            fprintf(stderr,"Option duplication error.\n\n" );
            usage(argv[0]);
        
         }
        //fprintf(stderr, BANNER);
            
        
        if(optind == argc) {
                fprintf(stderr, "No input file specified.\n\n");
                usage(argv[0]);
        }
        //int i;
        //for(i=0;i<)
        if((report==0)&(collate==0)){
                        fprintf(stderr, "Either -r or -c need to be at first.\n\n");
                        usage(argv[0]);
                    }
        char *ifile = argv[optind];
        if(report == collate) {
                fprintf(stderr, "Exactly one of '%s' or '%s' is required.\n\n",
                        option_table[REPORT].name, option_table[COLLATE].name);
                usage(argv[0]);
        }

        fprintf(stderr, "Reading input data...\n");
        c = readfile(ifile);
        if(errors) {
           printf("%d error%s found, so no computations were performed.\n",
                  errors, errors == 1 ? " was": "s were");
           exit(EXIT_FAILURE);
        }

        fprintf(stderr, "Calculating statistics...\n");
        s = statistics(c);
        if(s == NULL) fatal("There is no data from which to generate reports.");
        //normalize(c, s);
        normalize(c);
        composites(c);
        sortrosters(c, comparename);
        checkfordups(c->roster);
        // if(outfile){fp=out_file;}
        // else{fp=stdout;}
        if(collate) {
                fprintf(stderr, "Dumping collated data...\n");
                writecourse(stdout, c);
                //writecourse(fp, c);
                //fclose(stdout);
                exit(errors ? EXIT_FAILURE : EXIT_SUCCESS);
        }
        sortrosters(c, compare);

        fprintf(stderr, "Producing reports...\n");
        //printf("%d\n", outfile);
        if(outfile==1){
        reportparams(stderr, ifile, c);
    }
    else
               reportparams(stdout, ifile, c);
        if(moments) reportmoments(stdout, s);
        if(composite) reportcomposites(stdout, c, nonames);
        if(freqs) reportfreqs(stdout, s);
        if(quantiles) reportquantiles(stdout, s);
        if(summaries) reportquantilesummaries(stdout, s);
        if(histograms) reporthistos(stdout, c, s);
        if(scores) reportscores(stdout, c, nonames);
        if(tabsep) reporttabs(stdout, c);
        // reportparams(fp, ifile, c);
        // if(moments) reportmoments(fp, s);
        // if(composite) reportcomposites(fp, c, nonames);
        // if(freqs) reportfreqs(fp, s);
        // if(quantiles) reportquantiles(fp, s);
        // if(summaries) reportquantilesummaries(fp, s);
        // if(histograms) reporthistos(fp, c, s);
        // if(scores) reportscores(fp, c, nonames);
        // if(tabsep) reporttabs(fp, c);
        fprintf(stderr, "\nProcessing complete.\n");
        if(outfile!=1){
        printf("%d warning%s issued.\n", warnings+errors,
               warnings+errors == 1? " was": "s were");}
        //fclose(stderr);
        exit(errors ? EXIT_FAILURE : EXIT_SUCCESS);
}

void usage(name)
char *name;
{
        struct option_info *opt;

        fprintf(stderr, "Usage: %s [options] <data file>\n", name);
        fprintf(stderr, "Valid options are:\n");
        for(unsigned int i = 0; i < NUM_OPTIONS-1; i++) {
                opt = &option_table[i];
                char optchr[5] = {' ', ' ', ' ', ' ', '\0'};
                if(opt->chr)
                  sprintf(optchr, "-%c, ", opt->chr);
                char arg[32];
                if(opt->has_arg)
                    sprintf(arg, " <%.10s>", opt->argname);
                else
                    sprintf(arg, "%.13s", "");
                fprintf(stderr, "\t%s--%-10s%-13s\t%s\n",
                            optchr, opt->name, arg, opt->descr);
                opt++;
        }
        exit(EXIT_FAILURE);
}
