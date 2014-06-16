#ifndef VIENNACL_IO_MATRIX_MARKET_HPP
#define VIENNACL_IO_MATRIX_MARKET_HPP

/* =========================================================================
   Copyright (c) 2010-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.
   Portions of this software are copyright by UChicago Argonne, LLC.

                            -----------------
                  ViennaCL - The Vienna Computing Library
                            -----------------

   Project Head:    Karl Rupp                   rupp@iue.tuwien.ac.at

   (A list of authors and contributors can be found in the PDF manual)

   License:         MIT (X11), see file LICENSE in the base directory
============================================================================= */


/** @file matrix_market.hpp
    @brief A reader and writer for the matrix market format is implemented here
*/

#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cctype>
#include "viennacl/tools/adapter.hpp"
#include "viennacl/traits/size.hpp"
#include "viennacl/traits/fill.hpp"

#include <QString>
#include <QFile>
#include <QTextStream>

namespace viennacl
{
  namespace io
  {
    //helper
    namespace detail
    {
      inline void trim(char * buffer, long max_size)
      {
        //trim at beginning of string
        long start = 0;
        for (long i=0; i<max_size; ++i)
        {
          if (buffer[i] == ' ')
            ++start;
          else
            break;
        }

        //trim at end of string
        long stop = start;
        for (long i=stop; i<max_size; ++i)
        {
          if (buffer[i] == 0)   //end of string
            break;

          if (buffer[i] != ' ')
            stop = i;
        }

        for (long i=0; i<=stop - start; ++i)
        {
          buffer[i] = buffer[start + i];
        }

        if (buffer[0] != ' ')
          buffer[stop - start + 1] = 0; //terminate string
        else
          buffer[0] = 0;
      }

      inline std::string tolower(std::string & s)
      {
        std::transform(s.begin(), s.end(), s.begin(), static_cast < int(*)(int) > (std::tolower));
        return s;
      }



    } //namespace

    ///////// reader ////////////

    /** @brief Reads a sparse or dense matrix from a file (MatrixMarket format)
    *
    * @param mat The matrix that is to be read
    * @param file Filename from which the matrix should be read
    * @param index_base The index base, typically 1
    * @tparam MatrixType A generic matrix type. Type requirements: size1() returns number of rows, size2() returns number columns, operator() writes array entries, resize() allows resizing the matrix.
    * @return Returns nonzero if file is read correctly
    */
    template <typename MatrixType>
    long read_matrix_market_file_impl(MatrixType & mat,
                                      const char * file,
                                      long index_base)
    {
      QString filename(file);
      QFile theFile(filename);
      QTextStream reader(&theFile);


      if(!theFile.exists()){
        std::cout << "file does not exist" << std::endl;
      }
      else{
        std::cout << "file found:" << theFile.fileName().toStdString()<<std::endl;
      }
      if(!theFile.open(QIODevice::ReadOnly)){
        std::cout << "cannot open file" << std::endl;
      }
      else{
        std::cout << "file open" << std::endl;
      }
      std::cout << "matrix file status:" << reader.status() << std::endl;


      typedef typename viennacl::result_of::cpu_value_type<typename viennacl::result_of::value_type<MatrixType>::type>::type    ScalarType;

      //std::cout << "Reading matrix market file" << std::endl;
      char buffer[1025];
//      const char * buffer;
//      std::ifstream reader(file);
      std::string token;
      long linenum = 0;
      bool symmetric = false;
      bool dense_format = false;
      bool is_header = true;
      long cur_row = 0;
      long cur_col = 0;
      long valid_entries = 0;
      long nnz = 0;


      if (reader.status()){
        std::cout << (int)reader.status() << std::endl;
        std::cout << "ViennaCL: Matrix Market Reader: Cannot open file " << file << std::endl;
        return EXIT_FAILURE;
      }

      while (!reader.atEnd())//status 0 means all is good
      {
        std::cout << "reading line:" << std::endl;
        // get a non-empty line
        do
        {
//          QString resultPathString = absoluteAppRootPath + "/testdata/rhs65025.txt";
//          const QByteArray resultStringHolder = (QDir::toNativeSeparators(resultPathString)).toUtf8();
//          const char * resultPath = resultStringHolder.constData();
//          reader.getline(buffer, 1024);
          QString currentLine = reader.readLine(1024);
          QByteArray currentLineData = currentLine.toUtf8();
          char * data = (char *) currentLineData.constData();
//          buffer = data;
          strcpy(data, buffer);
//          for(int i=0;i<sizeof(buffer)/sizeof(char); i++){
//            std::cout << buffer[i];
//          }
          std::cout << buffer << std::endl;
          ++linenum;
          detail::trim(buffer, 1024);
        }
        while (!reader.status() && buffer[0] == 0);

        if (buffer[0] == '%')
        {
          if (buffer[1] == '%')
          {
            //parse header:
            std::stringstream line(std::string(buffer + 2));
            line >> token;
            if (detail::tolower(token) != "matrixmarket")
            {
              std::cerr << "Error in file " << file << " at line " << linenum << " in file " << file << ": Expected 'MatrixMarket', got '" << token << "'" << std::endl;
              return 0;
            }

            line >> token;
            if (detail::tolower(token) != "matrix")
            {
              std::cerr << "Error in file " << file << " at line " << linenum << " in file " << file << ": Expected 'matrix', got '" << token << "'" << std::endl;
              return 0;
            }

            line >> token;
            if (detail::tolower(token) != "coordinate")
            {
              if (detail::tolower(token) == "array")
              {
                dense_format = true;
                std::cerr << "Error in file " << file << " at line " << linenum << " in file " << file << ": 'array' type is not supported yet!" << std::endl;
                return 0;
              }
              else
              {
                std::cerr << "Error in file " << file << " at line " << linenum << " in file " << file << ": Expected 'array' or 'coordinate', got '" << token << "'" << std::endl;
                return 0;
              }
            }

            line >> token;
            if (detail::tolower(token) != "real")
            {
              std::cerr << "Error in file " << file << ": The MatrixMarket reader provided with ViennaCL supports only real valued floating point arithmetic." << std::endl;
              return 0;
            }

            line >> token;
            if (detail::tolower(token) == "general"){ }
            else if (detail::tolower(token) == "symmetric"){ symmetric = true; }
            else
            {
              std::cerr << "Error in file " << file << ": The MatrixMarket reader provided with ViennaCL supports only general or symmetric matrices." << std::endl;
              return 0;
            }

          }
        }
        else
        {
          std::stringstream line(std::stringstream::in | std::stringstream::out);
          line << std::string(buffer);

          if (is_header)
          {
            //read header line
            std::size_t rows;
            std::size_t cols;

            if (line.good())
              line >> rows;
            else
            {
              std::cerr << "Error in file " << file << ": Could not get matrix dimensions (rows) in line " << linenum << std::endl;
              return 0;
            }

            if (line.good())
              line >> cols;
            else
            {
              std::cerr << "Error in file " << file << ": Could not get matrix dimensions (columns) in line " << linenum << std::endl;
              return 0;
            }
            if (!dense_format)
            {
              if (line.good())
                line >> nnz;
              else
              {
                std::cerr << "Error in file " << file << ": Could not get matrix dimensions (columns) in line " << linenum << std::endl;
                return 0;
              }
            }

            if (rows > 0 && cols > 0)
              viennacl::traits::resize(mat, rows, cols);

            is_header = false;
          }
          else
          {
            //read data
            if (dense_format)
            {
              ScalarType value;
              line >> value;
              viennacl::traits::fill(mat, static_cast<vcl_size_t>(cur_row), static_cast<vcl_size_t>(cur_col), value);

              if (++cur_row == static_cast<long>(viennacl::traits::size1(mat)))
              {
                //next column
                ++cur_col;
                cur_row = 0;
              }
            }
            else //sparse format
            {
              long row;
              long col;
              ScalarType value;

              //parse data:
              if (line.good())
                line >> row;
              else
              {
                std::cerr << "Error in file " << file << ": Parse error for matrix entry in line " << linenum << std::endl;
                return 0;
              }

              if (line.good())
                line >> col;
              else
              {
                std::cerr << "Error in file " << file << ": Parse error for matrix entry in line " << linenum << std::endl;
                return 0;
              }

              //take index_base base into account:
              row -= index_base;
              col -= index_base;

              if (line.good())
                line >> value;
              else
              {
                std::cerr << "Error in file " << file << ": Parse error for matrix entry in line " << linenum << std::endl;
                return 0;
              }

              if (row >= static_cast<long>(viennacl::traits::size1(mat)) || row < 0)
              {
                std::cerr << "Error in file " << file << " at line " << linenum << ": Row index out of bounds: " << row << " (matrix dim: " << viennacl::traits::size1(mat) << " x " << viennacl::traits::size2(mat) << ")" << std::endl;
                return 0;
              }

              if (col >= static_cast<long>(viennacl::traits::size2(mat)) || col < 0)
              {
                std::cerr << "Error in file " << file << " at line " << linenum << ": Column index out of bounds: " << col << " (matrix dim: " << viennacl::traits::size1(mat) << " x " << viennacl::traits::size2(mat) << ")" << std::endl;
                return 0;
              }

              viennacl::traits::fill(mat, static_cast<vcl_size_t>(row), static_cast<vcl_size_t>(col), value); //basically equivalent to mat(row, col) = value;
              if (symmetric)
                viennacl::traits::fill(mat, static_cast<vcl_size_t>(col), static_cast<vcl_size_t>(row), value); //basically equivalent to mat(col, row) = value;

              if (++valid_entries == nnz)
                break;

            } //else dense_format
          }
        }
      }

      //std::cout << linenum << " lines read." << std::endl;
//      reader.close();
      reader.flush();
      theFile.close();
      return linenum;
    }


    /** @brief Reads a sparse matrix from a file (MatrixMarket format)
    *
    * @param mat The matrix that is to be read (ublas-types and std::vector< std::map <unsigned int, ScalarType> > are supported)
    * @param file The filename
    * @param index_base The index base, typically 1
    * @tparam MatrixType A generic matrix type. Type requirements: size1() returns number of rows, size2() returns number columns, operator() writes array entries, resize() allows resizing the matrix.
    * @return Returns nonzero if file is read correctly
    */
    template <typename MatrixType>
    long read_matrix_market_file(MatrixType & mat,
                                 const char * file,
                                 long index_base = 1)
    {
      std::cout << "int ream matrix market file1" << std::endl;
      return read_matrix_market_file_impl(mat, file, index_base);
    }

    template <typename MatrixType>
    long read_matrix_market_file(MatrixType & mat,
                                 const std::string & file,
                                 long index_base = 1)
    {
      std::cout << "int ream matrix market file2" << std::endl;
      return read_matrix_market_file_impl(mat, file.c_str(), index_base);
    }

    template <typename ScalarType>
    long read_matrix_market_file(std::vector< std::map<unsigned int, ScalarType> > & mat,
                                 const char * file,
                                 long index_base = 1)
    {
      std::cout << "int ream matrix market file3" << std::endl;
      viennacl::tools::sparse_matrix_adapter<ScalarType> adapted_matrix(mat);
      return read_matrix_market_file_impl(adapted_matrix, file, index_base);
    }

    template <typename ScalarType>
    long read_matrix_market_file(std::vector< std::map<unsigned int, ScalarType> > & mat,
                                 const std::string & file,
                                 long index_base = 1)
    {
      std::cout << "int ream matrix market file4" << std::endl;
      viennacl::tools::sparse_matrix_adapter<ScalarType> adapted_matrix(mat);
      return read_matrix_market_file_impl(adapted_matrix, file.c_str(), index_base);
    }


    ////////// writer /////////////
    template <typename MatrixType>
    void write_matrix_market_file_impl(MatrixType const & mat, const char * file, long index_base)
    {
      std::ofstream writer(file);

      long num_entries = 0;
      for (typename MatrixType::const_iterator1 row_it = mat.begin1();
            row_it != mat.end1();
            ++row_it)
        for (typename MatrixType::const_iterator2 col_it = row_it.begin();
              col_it != row_it.end();
              ++col_it)
          ++num_entries;

      writer << "%%MatrixMarket matrix coordinate real general" << std::endl;
      writer << mat.size1() << " " << mat.size2() << " " << num_entries << std::endl;

      for (typename MatrixType::const_iterator1 row_it = mat.begin1();
            row_it != mat.end1();
            ++row_it)
        for (typename MatrixType::const_iterator2 col_it = row_it.begin();
              col_it != row_it.end();
              ++col_it)
          writer << col_it.index1() + index_base << " " << col_it.index2() + index_base << " " << *col_it << std::endl;

      writer.close();
    }

    template <typename ScalarType>
    void write_matrix_market_file(std::vector< std::map<unsigned int, ScalarType> > const & mat,
                                  const char * file,
                                  long index_base = 1)
    {
      viennacl::tools::const_sparse_matrix_adapter<ScalarType> adapted_matrix(mat);
      return write_matrix_market_file_impl(adapted_matrix, file, index_base);
    }

    template <typename ScalarType>
    void write_matrix_market_file(std::vector< std::map<unsigned int, ScalarType> > const & mat,
                                  const std::string & file,
                                  long index_base = 1)
    {
      viennacl::tools::const_sparse_matrix_adapter<ScalarType> adapted_matrix(mat);
      return write_matrix_market_file_impl(adapted_matrix, file.c_str(), index_base);
    }

    /** @brief Writes a sparse matrix to a file (MatrixMarket format)
    *
    * @param mat The matrix that is to be read (ublas-types and std::vector< std::map <unsigned int, ScalarType> > are supported)
    * @param file The filename
    * @param index_base The index base, typically 1
    * @tparam MatrixType A generic matrix type. Type requirements: size1() returns number of rows, size2() returns number columns, operator() writes array entries, resize() allows resizing the matrix.
    * @return Returns nonzero if file is read correctly
    */
    template <typename MatrixType>
    void write_matrix_market_file(MatrixType const & mat,
                                  const std::string & file,
                                  long index_base = 1)
    {
      write_matrix_market_file_impl(mat, file.c_str(), index_base);
    }


  } //namespace io
} //namespace viennacl

#endif
