import React from 'react'
import { connect } from 'react-redux'
import {
  addSubreddit,
  selectSubreddit,
  fetchPostsIfNeeded
} from '../actions'

let AddSubreddit = ({ dispatch }) => {
  let input

  return(
    <div>
      <form onSubmit={e => {
        e.preventDefault()

        const concat = input.value.replace(/\s/g, "")
        if(!concat) {
          return
        }

        dispatch(addSubreddit(concat))
        dispatch(selectSubreddit(concat))
        dispatch(fetchPostsIfNeeded(concat))
        input.value = ''
      }}>
        <label>
          add subreddit:&nbsp;
          <input ref={node => {
            input = node
          }} />
        </label>
        <button type="submit">+</button>
      </form>
    </div>
  )
}

AddSubreddit = connect()(AddSubreddit)

export default AddSubreddit
