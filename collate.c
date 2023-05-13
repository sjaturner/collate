#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

struct line_fields
{
   struct line_fields *next;
   uint32_t token_count;
   char **tokens;
};

struct line_fields *head;
struct line_fields *tail;

enum
{
   FIELDS = 16,
   LINE_CHARS = 2048,
   LINE_NELEM = LINE_CHARS + 1,
};

int main(int argc, char *argv[])
{
   uint32_t fields[FIELDS] = { };
   uint32_t field_elems = 0;
   uint32_t max_field = 0;
   char field_set[LINE_CHARS] = { };

   for (uint32_t index = 1; index < (uint32_t)argc; ++index)
   {
      int field = strtol(argv[index], 0, 0) - 1;

      if (field < 0)
      {
         printf("fields start at one, like cut\n");
         exit(EXIT_FAILURE);
      }

      fields[field_elems++] = field;
      field_set[field] = 1;
      max_field = (uint32_t)field > max_field ? (uint32_t)field : max_field;
   }

   char line[LINE_NELEM] = { };

   while (fgets(line, sizeof line - 1, stdin))
   {
      uint32_t chars = strlen(line);
      char *string = strdup(line);
      char **tokens = calloc(chars, sizeof(char *));
      uint32_t token_count = 0;

      char *curr = string;
      for (;;)
      {
         char *token = strtok(curr, "\r\n ");

         if (!token)
         {
            break;
         }

         curr = 0;

         tokens[token_count++] = token;
      }

      tokens = realloc(tokens, token_count * sizeof(char *));
      struct line_fields *line_fields = calloc(1, sizeof(*line_fields));

      line_fields->tokens = tokens;
      line_fields->token_count = token_count;
      if (!head)
      {
         head = line_fields;
         tail = line_fields;
      }

      tail->next = line_fields;
      tail = line_fields;
   }

   if (0)
   {
      for (struct line_fields * line_fields = head; line_fields; line_fields = line_fields->next)
      {
         for (uint32_t index = 0; index < line_fields->token_count; ++index)
         {
            printf("%s:", line_fields->tokens[index]);
         }
         printf("\n");
      }
   }

   for (struct line_fields * outer = head; outer; outer = outer->next)
   {
      if (!outer->tokens)
      {
         continue;
      }
      if (outer->token_count < max_field)
      {
         continue;
      }

      for (uint32_t index = 0; index < outer->token_count; ++index)
      {
         if (field_set[index])
         {
            printf("%s ", outer->tokens[index]);
         }
      }

      for (uint32_t index = 0; index < outer->token_count; ++index)
      {
         if (!field_set[index])
         {
            printf("%s ", outer->tokens[index]);
         }
      }

      for (struct line_fields * inner = outer->next; inner; inner = inner->next)
      {
         if (!inner->tokens)
         {
            continue;
         }

         if (inner->token_count < max_field)
         {
            inner->tokens = 0;
            continue;
         }

         int match = 1;
         for (uint32_t index = 0; index < field_elems; ++index)
         {
            if (strcmp(outer->tokens[fields[index]], inner->tokens[fields[index]]))
            {
               match = 0;
               break;
            }
         }

         if (match)
         {
            for (uint32_t index = 0; index < inner->token_count; ++index)
            {
               if (!field_set[index])
               {
                  printf("%s ", inner->tokens[index]);
               }
            }

            inner->tokens = 0;
         }
      }

      printf("\n");
   }

   return 0;
}
